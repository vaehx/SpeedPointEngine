
#include <cstring>
#include <ChunkedObjectPool.h>
#include <vector>


////////////////////////////////////////////////////////////////////////////////////////////////
// TODO:

//	- Shadows: Shadow Caster?
//	- Refactor RenderEntity -> RenderObject

////////////////////////////////////////////////////////////////////////////////////////////////

#define IS_VALID_PTR(x) ((x) != 0)

struct Mat4f
{
};

struct Vec3f
{
};

struct Color3f
{
};

struct AABB
{
};

struct ViewFrustum
{
};

namespace Geo
{
	bool AABBInViewFrustum(const AABB& aabb, const ViewFrustum& camera)
	{
		// TODO
		return true;
	}
}


struct SCamera
{
	ViewFrustum viewFrustum;
};












//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	IRenderer
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

enum EZPassEffect
{
	eZPASS_DEFAULT,
	eZPASS_VEGETATION // includes bending in VS	
};

enum EShadingModel
{
	eSHADING_BLINN,
	eSHADING_COOK_TORRANCE
};


struct ITexture;
struct SShaderResources
{
	EShadingModel shadingModel;
	ITexture* pTextureMap;
	ITexture* pNormalMap;
	ITexture* pEnvironmentMap;

	float specularity; // blinn exponent & cook-torrance slope distribution parameter
	float refractiveIndex; // cook-torrance fresnel effect
};

struct IVertexBuffer;
struct IIndexBuffer;
struct SDrawCallDesc
{
	SShaderResources shaderResources;
	unsigned long vtxStart, vtxEnd;
	unsigned long idxStart, idxEnd;
};

struct SRenderDesc
{
	EZPassEffect zpassEffect;
	IVertexBuffer* pVB;
	IIndexBuffer* pIB;
	SDrawCallDesc* pSubsets;
	unsigned int nSubsets;
	Mat4f worldMtx;
};
struct STerrainRenderDesc
{
};

struct SParticleRenderDesc
{

};

struct SParticleSystemRenderDesc
{
	// Maybe eliminate the VB / IB here using some magic
	IVertexBuffer* pVB;
	IIndexBuffer* pIB;

	unsigned int nParticles;
	SParticleRenderDesc* pParticles; // nParticles
};


enum ELightSourceType
{
	eLIGHTSOURCE_OMNIDIRECTIONAL,
	eLIGHTSOURCE_SPOT,
	eLIGHTSOURCE_DIRECTIONAL
};

struct SLightDesc
{	
	SRenderDesc renderDesc; // Contains geometry i.e. shape of light
	Mat4f transform; // for deferred only, should not include translation
	
	float radius;
	Vec3f position;
	Vec3f direction;
	Color3f intensity; // spectral intensity of the light
};

struct SRenderLight
{
	SLightDesc lightDesc;

};

struct SRenderEntity
{
	SRenderDesc renderDesc;
	AABB aabb;
	SRenderLight affectingLights[4]; // only used for forward rendering
	int nAffectingLights;

	SRenderEntity()
		: nAffectingLights(0)
	{
		memset(&affectingLights, 0, sizeof(affectingLights));
	}
};


struct IRenderer
{
	virtual void BeginScene() = 0;
	virtual void EndScene() = 0;
	
	// render lights stored in RenderEntity for forward rendering
	virtual void Render(const SRenderEntity& pRenderDesc) = 0;

	virtual void RenderTerrain(STerrainRenderDesc* pTerrainRenderDesc) = 0;
	virtual void RenderDeferredLight(const SRenderLight& light) = 0;
};











//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Static Object
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

class CStatic
{
private:
	SRenderDesc m_RenderDesc;

public:
	// Static object does not move, so the render desc will not change
	SRenderDesc* GetRenderDesc();
};

SRenderDesc* CStatic::GetRenderDesc()
{
	return &m_RenderDesc;
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Light
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////


class CLight
{
private:
	SLightDesc m_LightDesc;
public:
	void GetLightDesc(SLightDesc* pDestination);
};

void CLight::GetLightDesc(SLightDesc* pDestination)
{
	if (IS_VALID_PTR(pDestination))
	{
		std::memcpy(pDestination, &m_LightDesc, sizeof(SLightDesc));
	}	
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	IObject / IEntity / Dynamic object
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

struct IRenderableComponent;
struct IPhysicalComponent;
struct IAnimateableComponent;
struct IScriptComponent;
struct ISoundComponent;

#define OBJ_TYPE_RENDERABLE 0x01 << 0
#define OBJ_TYPE_PHYSICAL 0x01 << 1
#define OBJ_TYPE_ANIMATEABLE 0x01 << 2
#define OBJ_TYPE_SCRIPTED 0x01 << 3
#define OBJ_TYPE_SOUND 0x01 << 4
#define OBJ_TYPE_PARTICLESYSTEM 0x01 << 5

struct IObject
{
	virtual unsigned long GetType() const = 0;

	virtual IRenderableComponent* GetRenderable() const = 0;
	virtual IPhysicalComponent* GetPhysical() const = 0;
	virtual IAnimateableComponent* GetAnimateable() const = 0;
	virtual IScriptComponent* GetScriptComponent() const = 0;
	virtual ISoundComponent* GetSoundComponent() const = 0;
};

// An entity is the same as an object (composed of components)
typedef struct IObject IEntity;


// Implemented by specific implementation of object
struct IRenderableComponent
{
	virtual void GetUpdatedRenderDesc(SRenderDesc* pDest) const = 0;
};

struct IParticleSystemRenderable : public IRenderableComponent
{
	virtual void GetUpdatedRenderDesc(SRenderDesc* pDest) const {}
	virtual SParticleSystemRenderDesc* GetUpdatedParticleSystemRenderDesc() = 0;
};

// Implemented by specific implementation of object
struct IPhysicalComponent
{
};

// Implemented by specific implementation of object
struct IAnimateableComponent
{
};

// Implemented by specific implementation of object
struct IScriptComponent
{
};

/*	
	RigidBody: Renderable + Physical
	Light: Not an entity
	Character: [Renderale + Physical] (inherits from RigidBody) + Animateable + Scriptable
	Usable: inherits from RigidBody + scriptable
	Decal: Renderable
	ParticleSystem: Renderable
	Vegetation: Renderable + Physical
*/


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Rigid Body
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

class CRigidBodyRenderable : public IRenderableComponent
{
private:
	SRenderDesc m_RenderDesc;
public:
	virtual void GetUpdatedRenderDesc(SRenderDesc* pDest) const;
};

void CRigidBodyRenderable::GetUpdatedRenderDesc(SRenderDesc* pDest) const
{
	// TODO:
	//	- Fill if necessary
	//	- Update transformation matrix
	
	memcpy(pDest, &m_RenderDesc, sizeof(SRenderDesc));	
}

class CRigidBody : public IObject
{
private:
	CRigidBodyRenderable m_Renderable;
	IPhysicalComponent* m_pPhysical;
public:
	virtual unsigned long GetType() const { return OBJ_TYPE_RENDERABLE | OBJ_TYPE_PHYSICAL; }

	virtual IRenderableComponent* GetRenderable() const { return (IRenderableComponent*)&m_Renderable; }
	virtual IPhysicalComponent* GetPhysical() const { return m_pPhysical; }
	virtual IAnimateableComponent* GetAnimateable() const { return 0; }
	virtual IScriptComponent* GetScriptComponent() const { return 0; }
};





//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Decal
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

class CDecalRenderable : public IRenderableComponent
{
private:
	SRenderDesc m_RenderDesc;
public:
	virtual void GetUpdatedRenderDesc(SRenderDesc* pDest) const;
};

void CDecalRenderable::GetUpdatedRenderDesc(SRenderDesc* pDest) const
{
	// TODO:
	//	- Fill if necessary
	//	- Set Decal as ZPass effect
	//	- Update transformation matrix
	memcpy(pDest, &m_RenderDesc, sizeof(SRenderDesc));
}

class CDecal : public IObject
{
private:
	IRenderableComponent* m_pRenderable;
public:
	virtual unsigned long GetType() const { return OBJ_TYPE_RENDERABLE; }

	virtual IRenderableComponent* GetRenderable() const { return m_pRenderable; }
	virtual IPhysicalComponent* GetPhysical() const { return 0; }
	virtual IAnimateableComponent* GetAnimateable() const { return 0; }
	virtual IScriptComponent* GetScriptComponent() const { return 0; }
};




//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Character
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

class CCharacter : public IObject
{
private:
	// using the same renderable as for the rigid body
	CRigidBodyRenderable m_Renderable;

	IPhysicalComponent* m_pPhysical;
	IAnimateableComponent* m_pAnimateable;
	IScriptComponent* m_pScriptComponent;
public:
	virtual unsigned long GetType() const { return OBJ_TYPE_RENDERABLE | OBJ_TYPE_PHYSICAL | OBJ_TYPE_ANIMATEABLE | OBJ_TYPE_SCRIPTED; }

	virtual IRenderableComponent* GetRenderable() const { return (IRenderableComponent*)&m_Renderable; }
	virtual IPhysicalComponent* GetPhysical() const { return m_pPhysical; }
	virtual IAnimateableComponent* GetAnimateable() const { return m_pAnimateable; }
	virtual IScriptComponent* GetScriptComponent() const { return m_pScriptComponent; }
};




//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Particle System
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////


// Solution:
//	- Use a SParticleSystemRenderDesc { pVB, pIB, texture } which containts a bunch of SParticleRenderDesc { position, rotation }
//	- 3DEngine detects that this object is a particle system, casts IRenderableComponent to IParticleSystemRenderable
class CParticleSystem;
class CParticleSystemRenderable : public IParticleSystemRenderable
{
private:
	CParticleSystem* m_pParticleSystem;
	SParticleSystemRenderDesc m_RenderDesc;
public:
	virtual SParticleSystemRenderDesc* GetUpdatedParticleSystemRenderDesc();
};

SParticleSystemRenderDesc* CParticleSystemRenderable::GetUpdatedParticleSystemRenderDesc()
{
	// TODO:
	//	- Modify SParticleRenderDescs according to stored and updated particle information from the CParticleSystem itself (keep pointer to it)
	return &m_RenderDesc;
}

class CParticleSystem : public IObject
{
private:
	CParticleSystemRenderable m_Renderable;

public:
	virtual unsigned long GetType() const { return OBJ_TYPE_RENDERABLE; }

	virtual IRenderableComponent* GetRenderable() const { return (IRenderableComponent*)&m_Renderable; }
	virtual IPhysicalComponent* GetPhysical() const { return 0; }
	virtual IAnimateableComponent* GetAnimateable() const { return 0; }
	virtual IScriptComponent* GetScriptComponent() const { return 0; }	
};











//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	IScene
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

enum ESceneNodeType
{
	eSCENE_NODE_STATIC = 0,
	eSCENE_NODE_ENTITY,
	eSCENE_NODE_LIGHT
};

struct SSceneNode
{
	AABB aabb;
	ESceneNodeType type;
	
	CStatic* pStatic; // only set if type == eSCENE_NODE_STATIC
	IObject* pObject; // only set if type == eSCENE_NODE_ENTITY
	CLight* pLight; // only set if type == eSCENE_NODE_LIGHT
};

struct IScene
{
	virtual std::vector<SSceneNode>* GetSceneNodes() = 0;
};






//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ITexture
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ITexture
{
};













//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CApplication
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

class CApplication
{
public:
	virtual void OnUpdate();
};







//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Framework (3DEngine + Core)
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

class CFramework
{
private:
	CApplication* m_pApplication;
	IScene* m_pScene;
	SCamera m_Camera;
	IRenderer* m_pRenderer;


	ChunkedObjectPool<SRenderEntity> m_RenderEntities;
	ChunkedObjectPool<SRenderLight> m_RenderLights;

	void CollectVisibleSceneNodes();

	// checks whether the object is visible and if so adds it to the list and searches dependencies (like affecting lights)
	void AddVisibleEntity(IEntity* pEntity, const AABB& aabb);
	void AddVisibleLight(CLight* pLight, const AABB& aabb);
	void AddVisibleStatic(CStatic* pStatic, const AABB& aabb);

public:
	void Update();
	void Render();
};


CFramework g_Framework;

void MainLoop()
{	
	g_Framework.Update();
	g_Framework.Render();
}

void CApplication::OnUpdate()
{
	// ...
}





void CFramework::CollectVisibleSceneNodes()
{


	// TODO: Find better algorithm that determines affected lights per object in a space-partitioning matter






	// Collect visible objects
	std::vector<SSceneNode>* pSceneNodes = m_pScene->GetSceneNodes();
	for (auto itSceneNode = pSceneNodes->begin(); itSceneNode != pSceneNodes->end(); itSceneNode++)
	{
		switch (itSceneNode->type)
		{
		case eSCENE_NODE_STATIC:
			AddVisibleStatic(itSceneNode->pStatic, itSceneNode->aabb);
			break;
		case eSCENE_NODE_ENTITY:
			AddVisibleEntity(itSceneNode->pObject, itSceneNode->aabb);
			break;


		case eSCENE_NODE_LIGHT:		 // !!! only used for forward rendering	!!!
			AddVisibleLight(itSceneNode->pLight, itSceneNode->aabb);
			break;

		default:
			break;
		}		
	}
}

void CFramework::AddVisibleEntity(IEntity* pEntity, const AABB& aabb)
{
	if (pEntity->GetType() & OBJ_TYPE_RENDERABLE <= 0)
	{
		return;
	}

	IRenderableComponent* pRenderable = pEntity->GetRenderable();

	if (Geo::AABBInViewFrustum(aabb, m_Camera.viewFrustum))
	{
		SRenderEntity* pRenderEntity = m_RenderEntities.Get();
		
		pRenderable->GetUpdatedRenderDesc(&pRenderEntity->renderDesc);
		pRenderEntity->aabb = aabb;
	}
}

void CFramework::AddVisibleLight(CLight* pLight, const AABB& aabb)
{
	// Add light for deferred light rendering
	SRenderLight* pRenderLight = m_RenderLights.Get();
	pLight->GetLightDesc(&pRenderLight->lightDesc);

	// Check found statics and entities if they are lit by this light - ONLY FOR FORWARD RENDERING
	unsigned int iterator = 0;
	SRenderEntity* pRenderEntity = 0;
	while (pRenderEntity = m_RenderEntities.GetNextUsedObject(iterator))
	{
		if (pRenderEntity->nAffectingLights >= 4 || pRenderEntity->renderDesc.pipeline != eRENDER_FORWARD)
			continue;

		SLightDesc* pLightDesc = &pRenderEntity->affectingLights[pRenderEntity->nAffectingLights].lightDesc;
		pLight->GetLightDesc(pLightDesc);

		pRenderEntity->nAffectingLights++;
	}
}

void CFramework::AddVisibleStatic(CStatic* pStatic, const AABB& aabb)
{

}



void CFramework::Update()
{
	m_pApplication->OnUpdate();
	CollectVisibleSceneNodes();
}

void CFramework::Render()
{
	// Render visible scene nodes
	unsigned int itRenderEntity = 0;
	SRenderEntity* pRenderEntity = 0;
	while (pRenderEntity = m_RenderEntities.GetNextUsedObject(itRenderEntity))
	{
		m_pRenderer->Render(*pRenderEntity);
	}

	unsigned int itRenderLight = 0;
	SRenderLight* pRenderLight = 0;
	while (pRenderLight = m_RenderLights.GetNextUsedObject(itRenderLight))
	{
		m_pRenderer->RenderDeferredLight(*pRenderLight);
	}
}





















//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	IRenderAPI
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

struct IRenderAPI
{
	virtual void SetRenderTarget(ERenderTarget rt) = 0;
	virtual void SetTexture(int slot, ITexture* pTexture) = 0;
	virtual void SetVertexBuffer(IVertexBuffer* pVertexBuffer) = 0;
	virtual void SetIndexBuffer(IIndexBuffer* pIndexBuffer) = 0;
	virtual void SetConstantsBuffer(int slot, EConstantsBuffer cb) = 0;
	virtual void SetActiveZPassShader(EZPassEffect zPassEffect) = 0;
	virtual void SetActiveShader(EShader shader) = 0;
	virtual void UpdateMaterialConstantBuffer(SMaterialConstantBuffer* pCB) = 0;
	virtual void UpdateObjectConstantBuffer(SObjectConstantBuffer* pCB) = 0;
	virtual void UpdateLightConstantBuffer(SLightConstantsBuffer* pCB) = 0;
	virtual void Draw(const SDrawCallDesc& dcd) = 0;
};








//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CRenderer
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

enum ERenderTarget
{
	eRT_BACKBUFFER,
	eRT_GBUFFER
};

enum EShader
{
	eSHADER_ZPASS_DEFAULT,
	eSHADER_ZPASS_VEGETATION,
	eSHADER_DEFERRED_SHADING,
	eSHADER_FORWARD
};

enum EConstantsBuffer
{
	eCONSTANTSBUFFER_NONE = 0,
	eCONSTANTSBUFFER_MATERIAL,
	eCONSTANTSBUFFER_OBJECT,
	eCONSTANTSBUFFER_LIGHT	// for deferred lights
};

// TODO: pad
struct SMaterialConstantBuffer
{
	EShadingModel shadingModel;
	float specularity; // used for blinn exponent and exponent for slope distribution function of Cook-Torrance
	float refractiveIndex; // used for cook-torrance model (fresnel effect)
};

// TODO: pad 
struct SObjectConstantBuffer
{
	Mat4f worldMatrix;
	Mat4f viewMatrix;
	Mat4f projectionMatrix;
};

// TODO: pad
struct SLightConstantsBuffer
{
	Mat4f transform;	// should not include translation
	Vec3f position;
	Vec3f direction;
	Color3f intensity; // spectral intensity
};

class CRenderer : public IRenderer
{
private:
	SMaterialConstantBuffer m_MaterialConstants;
	SObjectConstantBuffer m_ObjectConstants;	
	SLightConstantsBuffer m_LightConstants;

	IRenderAPI* m_pRenderAPI;

public:
	virtual void BeginScene();
	virtual void EndScene();

	virtual void Render(const SRenderEntity& entity);
	virtual void RenderTerrain(STerrainRenderDesc* pTerrainRenderDesc);
	virtual void RenderDeferredLight(const SRenderLight& light);
};


void CRenderer::Render(const SRenderEntity& entity)
{	
	// Forward:
	//	1. Set Backbuffer as active Render Target (Therefore: Render forward objects AFTER deferred shading)
	//	2. Set material: Texturemap, Normalmap
	//	3. For each light, draw the subsets of the object
	// Deferred Shading:
	//	1. Set GBuffer as active Render Target
	//	2. Set material
	//	3. Draw subsets of the object: Put  Depth, Normal, WPos and specularity
	//	Remember to call RenderDeferredLights() afterwards

	// --------------------------------------------------------------------------------
	// We'll just do deferred rendering here:

	const SRenderDesc* pRenderDesc = &entity.renderDesc;

	m_pRenderAPI->SetRenderTarget(eRT_GBUFFER);

	m_pRenderAPI->SetActiveZPassShader(pRenderDesc->zpassEffect);

	m_pRenderAPI->SetVertexBuffer(pRenderDesc->pVB);
	m_pRenderAPI->SetIndexBuffer(pRenderDesc->pIB);

	m_ObjectConstants.worldMatrix = pRenderDesc->worldMtx;
	m_pRenderAPI->UpdateObjectConstantBuffer(&m_ObjectConstants);
	m_pRenderAPI->SetConstantsBuffer(0, eCONSTANTSBUFFER_OBJECT);

	for (unsigned int i = 0; i < pRenderDesc->nSubsets; ++i)
	{
		SDrawCallDesc& dcd = pRenderDesc->pSubsets[i];

		SShaderResources* pShaderResources = &dcd.shaderResources;
		m_pRenderAPI->SetTexture(0, pShaderResources->pTextureMap);
		m_pRenderAPI->SetTexture(1, pShaderResources->pNormalMap);
		m_pRenderAPI->SetTexture(2, pShaderResources->pEnvironmentMap);

		m_MaterialConstants.shadingModel = pShaderResources->shadingModel;
		m_MaterialConstants.specularity = pShaderResources->specularity;
		m_MaterialConstants.refractiveIndex = pShaderResources->refractiveIndex;
		m_pRenderAPI->UpdateMaterialConstantBuffer(&m_MaterialConstants);
		m_pRenderAPI->SetConstantsBuffer(1, eCONSTANTSBUFFER_MATERIAL);

		m_pRenderAPI->Draw(dcd);
	}
}

void CRenderer::RenderDeferredLight(const SRenderLight& light)
{
	const SRenderDesc* pLightRenderDesc = &light.lightDesc.renderDesc;
	if (pLightRenderDesc->nSubsets == 0 || !IS_VALID_PTR(pLightRenderDesc->pSubsets))
	{
		return;
	}

	m_pRenderAPI->SetActiveShader(eSHADER_DEFERRED_SHADING);
	m_pRenderAPI->SetRenderTarget(eRT_BACKBUFFER);

	m_pRenderAPI->SetVertexBuffer(pLightRenderDesc->pVB);
	m_pRenderAPI->SetIndexBuffer(pLightRenderDesc->pIB);

	m_LightConstants.transform = light.lightDesc.transform;
	m_LightConstants.position = light.lightDesc.position;
	m_LightConstants.direction = light.lightDesc.direction;
	m_LightConstants.intensity = light.lightDesc.intensity;
	m_pRenderAPI->UpdateLightConstantBuffer(&m_LightConstants);

	m_pRenderAPI->SetConstantsBuffer(0, eCONSTANTSBUFFER_LIGHT);
	m_pRenderAPI->SetConstantsBuffer(1, eCONSTANTSBUFFER_NONE);

	SDrawCallDesc& dd = pLightRenderDesc->pSubsets[0];
	m_pRenderAPI->SetTexture(0, 0);
	m_pRenderAPI->SetTexture(1, 0);
	m_pRenderAPI->SetTexture(2, 0);

	m_pRenderAPI->Draw(dd);
}