What's up with the Render()-Method???

#1 Keep Render() as a method for rendering without static entry in the render schedule

#2 Add new function IRenderableObject::PrepareRenderDesc() for whenever the object is to be added into the render-schedule
	Scene::RegisterObject(IRenderableObject* pObject)
	{
		// prepare the render desc to be added into the render-schedule
		SRenderDesc renderDesc = pObject->PrepareRenderDesc();

		RenderScheduleSlot* pSlot = pRenderer->GetRenderScheduleSlot();
		if (IS_VALID_PTR(pSlot))	
			pSlot->renderDesc = renderDesc;	
	}

	--> Problem: How can we update the renderdesc? (E.g. to update transformation?)
		- Assuming each object can only have one render schedule slot:
			- Store pointer to the RenderScheduleSlot in the object (in the renderable component)
				IRenderableObject::SetRenderScheduleSlot(RenderScheduleSlot* pSlot)
				{
					m_Renderable.pRSSlot = pSlot;
				}
			- Add function IRenderableObject::UpdateRenderScheduleSlot(), which syncs (e.g. transform)
			  with the RenderScheduleSlot.
			- Maybe use Render() as the function that updates the render schedule slot?
		- Assuming an object can add multiple RenderScheduleSlots:
			- Each RS of a specific object has to be handled like an actual instance.
			- Where to store these instances?
			--> Inverse Update:
				Store pointer back to IRenderableObject in the renderdesc.
				Add IRenderableComponent::UpdateRenderScheduleSlot(RenderScheduleSlot& slot).
				Whenever the renderschedule is unleashed, UpdateRenderScheduleSlot(curSlot) of
				curSlot.pRenderable ist invoked.
			BUT: Multiple instances not even possible (one object carries only one transformation)
			- Object data not stored in the RenderableComponent itself. Component contains just pointers
   			  to the data (IB, VB, Materials). Multiple instances in RAM are simply represented by objects
			  pointing to the same data. Work out some method to appropriately mark them as instances
			  of the one data during rendering.

		==> Prefer easier method: Each object may only have ne render schedule slot. :)

#3 Keep Render(). If has RenderSlot already, just update it, otherwise add it to the RenderSchedule.	
	- SDrawCallDesc: Remove transform member.
	struct SRenderSubset
	{
		SMaterial material;	// copied, no pointer used
		SDrawCallDesc drawCallDesc;
		bool render; // true to render, false to skip
	};
	- Rename ERenderPipeline Technique to ERenderPipeline
	struct SRenderDesc
	{
		ERenderPipeline renderPipeline;
		SRenderSubset* pSubsets;
		unsigned int nSubsets;
		
		SMaterial material;
		STransformationDesc transform;
		//IGeometry* pGeometry;
	};

	struct IRenderableComponent
	{
		virtual SRenderSlot* GetRenderSlot() = 0;
		virtual void SetRenderSlot(SRenderSlot* pSlot) = 0;
		
		virtual IVertexBuffer* GetVertexBuffer() = 0;
		virtual SGeomSubset* GetSubset(unsigned int i) = 0;
		virtual unsigned int GetSubsetCount() const = 0;
		virtual EPrimitiveType GetGeometryPrimitiveType() const = 0;
	};

	void Object::Render()
	{
		SRenderSlot* pRenderSlot = m_Renderable.GetRenderSlot();
		SRenderDesc* pRenderDesc = 0;

		// Add RenderScheduleSlot if not there already
		if (m_Renderable.GetRenderSlot() == 0)
		{
			pRenderSlot = pRenderer->GetRenderScheduleSlot();
			pRenderDesc = &pRenderSlot->renderDesc;		

			// copy over subsets
			pRenderDesc->nSubsets = m_Renderable.GetSubsetCount();
			pRenderDesc->pSubsets = new SRenderSubset[pRenderDesc->nSubsets];
			for (unsigned int i = 0; i < pRenderDesc->nSubsets; ++i)
			{
				SRenderSubset& renderSubset = pRenderDesc->pSubsets[i];
				SGeomSubset& subset = m_Renderable->GetSubset(i);

				renderSubset.drawCallDesc.pVertexBuffer = m_Renderable->GetVertexBuffer();
				renderSubset.drawCallDesc.pIndexBuffer = subset.pIndexBuffer;
				
				renderSubset.render = false;
				if (IS_VALID_PTR(renderSubset.drawCallDesc.pIndexBuffer) && IS_VALID_PTR(renderSubset.drawCallDesc.pVertexBuffer))
				{
					renderSubset.drawCallDesc.iStartIBIndex = 0;
					renderSubset.drawCallDesc.iEndIBIndex = dcd.pIndexBuffer->GetIndexCount() - 1;
					renderSubset.drawCallDesc.iStartVBIndex = 0;
					renderSubset.drawCallDesc.iEndVBIndex = dcd.pVertexBuffer->GetVertexCount() - 1;

					renderSubset.render = true;
				}		

				renderSubset.drawCallDesc.primitiveType = m_Renderable.GetGeometryPrimitiveType();

				// Material is copied over. Warning: Avoid modifying the material during rendering.
				// TODO: To update a material, implement a method to flag a subset material to be updated,
				// 	 then do it below when updating the transform.
				if (subset.pMaterial)
					renderSubset.material = *subset.pMaterial;
				else
					renderSubset.material = m_pEngine->GetResources()->GetDefaultMaterial();								
			}
		}
		else
		{
			pRenderDesc = &pRenderSlot->renderDesc;
		}

		// set / update transformation
		STransformationDesc transformDesc;
		transformDesc.translation = SMatrix::MakeTranslationMatrix(vPosition);
		transformDesc.rotation = SMatrix::MakeRotationMatrix(vRotation);
		transformDesc.scale = SMatrix::MakeScaleMatrix(vSize);
	}

	- Remove Renderer::RenderGeometry() and Renderer::RenderTerrain() if still there.


	void Renderer::UnleashRenderSchedule()
	{
		unsigned int iRSIterator = 0;
		for (unsigned int iSlot = 0; iSlot < m_RenderSchedule.GetUsedObjectCount(); ++iSlot)
		{
			SRenderScheduleSlot* pSlot = m_RenderSchedule.GetNextUsedObject(iRSIterator);
			if (!IS_VALID_PTR(pSlot))
				continue;
			
			
		}		
	}

	void Renderer::RenderSlotForward()
	{
		BindSingleFBO(m_pTargetViewport);	// BindSingleFBO(m_pForwardRT);
		
	}


	// Due to device capabilities this might be restrained to a value < 8, but this is not
	// a problem as we just store pointers to FBOS. We'll just leave the memory untouched.
	#define MAX_BOUND_FBOS 8

	class Renderer
	{
	private:
		IFBO* m_pBoundFBOs[MAX_BOUND_FBOS];
		unsigned int m_nBoundFBOs;
	};

	void Renderer::BindSingleFBO(IViewport* pViewport)
	{
		BindSingleFBO(pViewport->GetFBO());
	}
	
	void Renderer::BindSingleFBO(IFBO* pFBO)
	{
		if (!BoundMultipleFBOs() && GetBoundSingleFBO() == pFBO)
			return;		

		DirectX11FBO* pSPDXFBO = dynamic_cast<DirectX11FBO*>(pFBO);
		ID3D11RenderTargetView* pRTV = pSPDXFBO->GetRTV();
		ID3D11DepthStenvilView pDSV = pSPDXFBO->GetDSV();

		m_pD3DDeviceContext->OMSetRenderTargets(1, &pRTV, pDSV);		
	}
	
	IFBO* Renderer::GetBoundSingleFBO()
	{
		if (BoundMultipleFBOs())
			EngLog(S_WARN, m_pEngine, "Queried Bound Single FBO, but multiple FBOs bound!");

		return m_pBoundFBOS[0];
	}

	// dsvFBO = FBO, whoes DepthStencil Buffer should be used
	SResult Renderer::BindFBOCollection(std::vector<IFBO*>& fboCollection, IFBO* dsvFBO = 0)
	{
		if (fboCollection.size() == 0)
			return S_INVALIDPARAM;

		ID3D11DepthStencilView* pDSV = IS_VALID_PTR(dsvFBO) ? dsvFBO->GetDSV() : 0;

		// Put all FBOs into 
		ID3D11RenderTargetView** pRTVs = new ID3D11RenderTargetView*[fboCollection.size()];
		for (auto itFBO = fboCollection.begin(); itFBO != fboCollection.end(); itFBO++)
			pRTVs[itFBO - fboCollection.begin()] = itFBO->GetRTV;

		// Bind the rtvs
		m_pD3DDeviceContext->OMSetRenderTargets(fboCollection.size(), pRTVs, pDSV);

		return S_SUCCESS;
	}

	bool Renderer::BoundMultipleFBOs() const { return m_nBoundFBOs > 1; }

-------------------------------------------------------------------------------------------------------------------

Multipass with access to previous pass output:
	1. Set render target to Texture A
	2. Render first pass
	3. Set render target to Texture B
	4. Setup hlsl sampler with texture A
	5. Render second pass (using texture A for sampling)



struct CompositedEffectPass
{
	SRenderDesc* pRenderDesc;
	
	void Draw()
	{
		... (Todo)
	}
};

class CompositedEffect
{
private:
	SRenderDesc* pRenderDesc;
	vecor<CompositedEffectPass> passes;
public:
	void Execute()
	{
		for (auto itPass = passes.begin(); itPass != passes.end(); itPass++)
		{
			itPass->Draw();
		}
	}
};