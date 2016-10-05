#include <Abstract\CHelper.h>
#include <Abstract\IResourcePool.h>
#include <Abstract\IVertexBuffer.h>
#include <Abstract\IIndexBuffer.h>
#include <Abstract\SVertex.h>
#include <Abstract\SColor.h>

SP_NMSPACE_BEG

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API CDynamicMeshHelper::CDynamicMeshHelper()	
	: CHelper(),
	m_Scale(1.0f)
{
	RecalcTransform();
}

S_API CDynamicMeshHelper::~CDynamicMeshHelper()
{
	ClearRenderDesc();
}

S_API SRenderDesc* CDynamicMeshHelper::GetDynamicMesh()
{
	return (IsTrash() ? 0 : &m_RenderDesc);
}

S_API void CDynamicMeshHelper::ClearRenderDesc()
{
	IResourcePool* pResources = SpeedPointEnv::GetEngine()->GetResources();

	if (IS_VALID_PTR(m_RenderDesc.pSubsets) && !IS_VALID_PTR(pResources))
	{
		SDrawCallDesc* dcd = &m_RenderDesc.pSubsets[0].drawCallDesc;
		pResources->RemoveVertexBuffer(&dcd->pVertexBuffer);
		pResources->RemoveIndexBuffer(&dcd->pIndexBuffer);
	}

	m_RenderDesc.Clear();
}

S_API void CDynamicMeshHelper::SetParams(const Params& params)
{
	IRenderer* pRenderer = SpeedPointEnv::GetEngine()->GetRenderer();
	IResourcePool* pResources = SpeedPointEnv::GetEngine()->GetResources();
	assert(IS_VALID_PTR(pRenderer));
	assert(IS_VALID_PTR(pResources));

	ClearRenderDesc();

	m_RenderDesc.bCustomViewProjMtx = false;
	m_RenderDesc.bDepthStencilEnable = true;
	m_RenderDesc.bInverseDepthTest = false;
	m_RenderDesc.nSubsets = 1;
	m_RenderDesc.pSubsets = new SRenderSubset[1];
	
	SRenderSubset* subset = &m_RenderDesc.pSubsets[0];
	subset->bOnce = false;
	subset->render = true;
	subset->shaderResources.illumModel = eILLUM_HELPER;
	
	SDrawCallDesc* dcd = &subset->drawCallDesc;
	dcd->primitiveType = params.topology;

	bool lines = (params.topology == PRIMITIVE_TYPE_LINES || params.topology == PRIMITIVE_TYPE_LINESTRIP);
	if (params.numVertices > 0 && IS_VALID_PTR(params.pVertices) && (lines || (!lines && params.numIndices > 0 && IS_VALID_PTR(params.pIndices))))
	{
		pResources->AddVertexBuffer(&dcd->pVertexBuffer);
		dcd->pVertexBuffer->Initialize(pRenderer, eVBUSAGE_DYNAMIC_RARE, params.pVertices, params.numVertices);
		dcd->iStartVBIndex = 0;
		dcd->iEndVBIndex = params.numVertices - 1;

		if (!lines)
		{
			pResources->AddIndexBuffer(&dcd->pIndexBuffer);
			dcd->pIndexBuffer->Initialize(pRenderer, eIBUSAGE_STATIC, params.pIndices, params.numIndices);
			dcd->iStartIBIndex = 0;
			dcd->iEndIBIndex = params.numIndices - 1;
		}
	}
	else
	{
		subset->render = false;
	}
}

S_API IVertexBuffer* CDynamicMeshHelper::GetVertexBuffer() const
{
	if (m_RenderDesc.nSubsets == 0 || !m_RenderDesc.pSubsets)
		return 0;

	return m_RenderDesc.pSubsets[0].drawCallDesc.pVertexBuffer;
}

S_API const Mat44& CDynamicMeshHelper::GetTransform() const
{
	return m_RenderDesc.transform;
}

S_API void CDynamicMeshHelper::SetTransform(const Mat44& transform)
{
	m_RenderDesc.transform = transform;
}

S_API void CDynamicMeshHelper::RecalcTransform()
{
	MakeTransformationTRS(m_Pos, m_Rotation.ToRotationMatrix(), m_Scale, &m_RenderDesc.transform);
}

S_API void CDynamicMeshHelper::SetPos(const Vec3f& pos)
{
	m_Pos = pos;
	RecalcTransform();
}

S_API Vec3f CDynamicMeshHelper::GetPos() const
{
	return m_Pos;
}


S_API void CDynamicMeshHelper::SetRotation(const Quat& rot)
{
	m_Rotation = rot;
	RecalcTransform();
}

S_API Quat CDynamicMeshHelper::GetRotation() const
{
	return m_Rotation;
}


S_API void CDynamicMeshHelper::SetScale(const Vec3f& scale)
{
	m_Scale = scale;
	RecalcTransform();
}

S_API Vec3f CDynamicMeshHelper::GetScale() const
{
	return m_Scale;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API const SHelperGeometryDesc* CPointHelper::GetBaseGeometry(bool outline)
{
	static SHelperGeometryDesc geom;
	static bool geomInited = false;

	if (!geomInited)
	{
		// Create a little diamond shape
		const float sz = 0.5f;						// length of one side of the diamond
		const float hsz = sz * 0.5f;				// half-sz
		const float hh = sqrtf(0.5f * sz * sz);	// half-height, i.e. distance from middle point to each point
		const float normhsz = hsz / hh;					// normalized half-sz

		geom.vertices = {
			/*0*/ SVertex(-hsz, 0, -hsz, -normhsz, 0, -normhsz, 0, 0, 0),
			/*1*/ SVertex(-hsz, 0, hsz, -normhsz, 0, normhsz, 0, 0, 0),
			/*2*/ SVertex(hsz, 0, hsz, normhsz, 0, normhsz, 0, 0, 0),
			/*3*/ SVertex(hsz, 0, -hsz, normhsz, 0, -normhsz, 0, 0, 0),
			/*4*/ SVertex(0, -hh, 0, 0, -1.0f, 0, 0, 0, 0),
			/*5*/ SVertex(0, hh, 0, 0, 1.0f, 0, 0, 0, 0)
		};

		// cw
		geom.indices = {
			0, 5, 3,
			3, 5, 2,
			2, 5, 1,
			1, 5, 0,

			0, 3, 4,
			3, 2, 4,
			2, 1, 4,
			1, 0, 4
		};

		geom.topology = PRIMITIVE_TYPE_TRIANGLELIST;
		geomInited = true;
	}

	return &geom;
}

S_API void CPointHelper::SetParams(const CPointHelper::Params& params)
{
	SetPos(params.pos);
}

S_API void CPointHelper::SetPos(const Vec3f& p)
{
	m_Params.pos = p;
	MakeTransformationTRS(p, Quat(), Vec3f(1.0f), &m_Transform);
}

S_API const Vec3f& CPointHelper::GetPos() const
{
	return m_Params.pos;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API const SHelperGeometryDesc* CLineHelper::GetBaseGeometry(bool outline)
{
	static SHelperGeometryDesc geom;
	static bool geomInited = false;

	if (!geomInited)
	{
		// Line of length 1 pointing upwards
		geom.topology = PRIMITIVE_TYPE_LINES;
		geom.vertices = {
			SVertex(0, 0, 0, 0, 0, 0, 0, 0, 0),
			SVertex(0, 1.0f, 0, 0, 0, 0, 0, 0, 0)
		};

		geomInited = true;
	}

	return &geom;
}

S_API void CLineHelper::RecalcTransform()
{
	Vec3f v = m_Params.p2 - m_Params.p1;
	float ln = v.Length();
	v /= ln;

	Quat q = Quat::FromVectors(Vec3f(0, 1, 0), v);
	Vec3f scale = Vec3f(1.0f, ln, 1.0f);

	//MakeTransformationTRS(m_Params.p1, q, scale, &m_Transform);
	MakeTransformationTRS(m_Params.p1, Quat(), Vec3f(1.0f), &m_Transform);
}

S_API void CLineHelper::SetParams(const CLineHelper::Params& params)
{
	m_Params = params;
	RecalcTransform();
}

S_API void CLineHelper::SetP1(const Vec3f& p1)
{
	m_Params.p1 = p1;
	RecalcTransform();
}

S_API const Vec3f& CLineHelper::GetP1() const
{
	return m_Params.p1;
}

S_API void CLineHelper::SetP2(const Vec3f& p2)
{
	m_Params.p2 = p2;
	RecalcTransform();
}

S_API const Vec3f& CLineHelper::GetP2() const
{
	return m_Params.p2;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API const SHelperGeometryDesc* CBoxHelper::GetBaseGeometry(bool outline)
{
	static SHelperGeometryDesc geom, outlineGeom;
	static bool geomInited = false;
	static bool outlineGeomInited = false;

	if (!geomInited)
	{
		// Per side:
		// 1 o-----o 2
		//   |   / |
		//   | /   |
		// 0 o-----o 3

		float s = 0.5f;
		geom.vertices = {
			// Front:
			/*0*/ SVertex(-s, -s, -s, 0, 0, -1.0f, 0, 0, 0),
			/*1*/ SVertex(-s,  s, -s, 0, 0, -1.0f, 0, 0, 0),
			/*2*/ SVertex(s,  s, -s, 0, 0, -1.0f, 0, 0, 0),
			/*3*/ SVertex(s, -s, -s, 0, 0, -1.0f, 0, 0, 0),

			// Right:
			/*4*/ SVertex(s, -s, -s, 1.0f, 0, 0, 0, 0, 0),
			/*5*/ SVertex(s,  s, -s, 1.0f, 0, 0, 0, 0, 0),
			/*6*/ SVertex(s,  s,  s, 1.0f, 0, 0, 0, 0, 0),
			/*7*/ SVertex(s, -s,  s, 1.0f, 0, 0, 0, 0, 0),

			// Back:
			/*8 */ SVertex(-s, -s, s, 0, 0, 1.0f, 0, 0, 0),
			/*9 */ SVertex(-s,  s, s, 0, 0, 1.0f, 0, 0, 0),
			/*10*/ SVertex(s,  s, s, 0, 0, 1.0f, 0, 0, 0),
			/*11*/ SVertex(s, -s, s, 0, 0, 1.0f, 0, 0, 0),

			// Left:
			/*12*/ SVertex(-s, -s, -s, -1.0f, 0, 0, 0, 0, 0),
			/*13*/ SVertex(-s,  s, -s, -1.0f, 0, 0, 0, 0, 0),
			/*14*/ SVertex(-s,  s,  s, -1.0f, 0, 0, 0, 0, 0),
			/*15*/ SVertex(-s, -s,  s, -1.0f, 0, 0, 0, 0, 0),

			// Top:
			/*16*/ SVertex(-s, s, -s, 0, 1.0f, 0, 0, 0, 0),
			/*17*/ SVertex(-s, s,  s, 0, 1.0f, 0, 0, 0, 0),
			/*18*/ SVertex(s, s,  s, 0, 1.0f, 0, 0, 0, 0),
			/*19*/ SVertex(s, s, -s, 0, 1.0f, 0, 0, 0, 0),

			// Bottom:
			/*20*/ SVertex(-s, -s, -s, 0, -1.0f, 0, 0, 0, 0),
			/*21*/ SVertex(-s, -s,  s, 0, -1.0f, 0, 0, 0, 0),
			/*22*/ SVertex(s, -s,  s, 0, -1.0f, 0, 0, 0, 0),
			/*23*/ SVertex(s, -s, -s, 0, -1.0f, 0, 0, 0, 0),
		};

		geom.indices.resize(2 * 3 * 6);
		for (int side = 0; side < 6; ++side)
		{
			geom.indices[side * 6] = (side * 4);
			geom.indices[side * 6 + 1] = (side * 4) + 1;
			geom.indices[side * 6 + 2] = (side * 4) + 2;
			geom.indices[side * 6 + 3] = (side * 4);
			geom.indices[side * 6 + 4] = (side * 4) + 2;
			geom.indices[side * 6 + 5] = (side * 4) + 3;
		}

		geom.topology = PRIMITIVE_TYPE_TRIANGLELIST;
		geomInited = true;
	}

	if (!outlineGeomInited)
	{
		float s = 0.5f;
		SVertex corners[8];
		corners[0] = SVertex(+s, +s, +s); // back left top
		corners[1] = SVertex(-s, +s, +s); // back right top
		corners[2] = SVertex(-s, +s, -s); // front right top
		corners[3] = SVertex(+s, +s, -s); // front left top		
		corners[4] = SVertex(+s, -s, +s); // back left bottom
		corners[5] = SVertex(-s, -s, +s); // back right bottom
		corners[6] = SVertex(-s, -s, -s); // front right bottom
		corners[7] = SVertex(+s, -s, -s); // front left bottom

		outlineGeom.vertices.resize(24);

		// top
		outlineGeom.vertices[0] = corners[0]; outlineGeom.vertices[1] = corners[1];
		outlineGeom.vertices[2] = corners[1]; outlineGeom.vertices[3] = corners[2];
		outlineGeom.vertices[4] = corners[2]; outlineGeom.vertices[5] = corners[3];
		outlineGeom.vertices[6] = corners[3]; outlineGeom.vertices[7] = corners[0];

		// bottom
		outlineGeom.vertices[8] = corners[4]; outlineGeom.vertices[9] = corners[5];
		outlineGeom.vertices[10] = corners[5]; outlineGeom.vertices[11] = corners[6];
		outlineGeom.vertices[12] = corners[6]; outlineGeom.vertices[13] = corners[7];
		outlineGeom.vertices[14] = corners[7]; outlineGeom.vertices[15] = corners[4];

		// sides
		outlineGeom.vertices[16] = corners[0]; outlineGeom.vertices[17] = corners[4];
		outlineGeom.vertices[18] = corners[1]; outlineGeom.vertices[19] = corners[5];
		outlineGeom.vertices[20] = corners[2]; outlineGeom.vertices[21] = corners[6];
		outlineGeom.vertices[22] = corners[3]; outlineGeom.vertices[23] = corners[7];

		outlineGeom.topology = PRIMITIVE_TYPE_LINES;

		outlineGeomInited = true;
	}

	return (outline ? &outlineGeom : &geom);
}

S_API void CBoxHelper::RecalcTransform()
{
	Mat44 rotation;
	float scale[3];
	for (int i = 0; i < 3; ++i)
	{
		scale[i] = Vec3Length(m_Params.dimensions[i]);

		Vec3f ndim = m_Params.dimensions[i] / scale[i];
		rotation.m[i][0] = ndim.x;
		rotation.m[i][1] = ndim.y;
		rotation.m[i][2] = ndim.z;
	}

	MakeTransformationTRS(m_Params.center, SMatrixTranspose(rotation), Vec3f(scale[0], scale[1], scale[2]) * 2.0f, &m_Transform);
}

S_API void CBoxHelper::SetParams(const CBoxHelper::Params& params)
{
	m_Params = params;
	RecalcTransform();
}

S_API void CBoxHelper::SetCenter(const Vec3f& center)
{
	m_Params.center = center;
	RecalcTransform();
}

S_API const Vec3f& CBoxHelper::GetCenter() const
{
	return m_Params.center;
}

S_API void CBoxHelper::SetDimensions(const Vec3f& dimX, const Vec3f& dimY, const Vec3f& dimZ)
{
	m_Params.dimensions[0] = dimX;
	m_Params.dimensions[1] = dimY;
	m_Params.dimensions[2] = dimZ;
	RecalcTransform();
}

S_API const Vec3f& CBoxHelper::GetDimX() const
{
	return m_Params.dimensions[0];
}

S_API const Vec3f& CBoxHelper::GetDimY() const
{
	return m_Params.dimensions[1];
}

S_API const Vec3f& CBoxHelper::GetDimZ() const
{
	return m_Params.dimensions[2];
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API const SHelperGeometryDesc* CSphereHelper::GetBaseGeometry(bool outline)
{
	static SHelperGeometryDesc geom;
	static bool geomInited = false;

	if (!geomInited)
	{
		const unsigned int nStripes = 15;
		const unsigned int nRings = 15;



		//TODO: Test if this is the correct value!! - or should we use 0.5 here? -> See RecalcTransform() of sphere as well!
		const float radius = 1.0f;



		geom.vertices.resize((nStripes + 1) * (nRings + 1));
		geom.indices.resize((nStripes * nRings) * 6);

		geom.topology = PRIMITIVE_TYPE_TRIANGLELIST;

		float dTheta = (float)SP_PI / (float)nRings;
		float dPhi = (float)SP_PI * 2.0f / (float)nStripes;

		float dU = 1.0f / (float)nRings;
		float dV = 1.0f / (float)nStripes;

		unsigned int indexAccum = 0;
		for (unsigned int ring = 0; ring <= nRings; ++ring)
		{
			float theta = (float)ring * dTheta;
			for (unsigned int stripe = 0; stripe <= nStripes; ++stripe)
			{
				float phi = (float)stripe * dPhi;

				Vec3f normal;
				normal.x = sinf(theta) * cosf(phi);
				normal.y = sinf(theta) * sinf(phi);
				normal.z = cosf(theta);

				geom.vertices[ring * (nStripes + 1) + stripe] = SVertex(
					radius * normal.x, radius * normal.y, radius * normal.z,
					normal.x, normal.y, normal.z, 0, 0, 0, (float)stripe * dU, (float)ring * dV);

				if (ring < nRings && stripe < nStripes)
				{
					geom.indices[indexAccum] = ring * (nStripes + 1) + stripe;
					geom.indices[indexAccum + 1] = geom.indices[indexAccum] + (nStripes + 1) + 1;
					geom.indices[indexAccum + 2] = geom.indices[indexAccum] + 1;
					geom.indices[indexAccum + 3] = geom.indices[indexAccum];
					geom.indices[indexAccum + 4] = geom.indices[indexAccum] + (nStripes + 1);
					geom.indices[indexAccum + 5] = geom.indices[indexAccum + 1];
					indexAccum += 6;
				}
			}
		}

		geomInited = true;
	}

	return &geom;
}

S_API void CSphereHelper::RecalcTransform()
{
	MakeTransformationTRS(m_Params.center, Quat(), Vec3f(m_Params.radius), &m_Transform);
}

S_API void CSphereHelper::SetParams(const CSphereHelper::Params& params)
{
	m_Params = params;
	RecalcTransform();
}

S_API void CSphereHelper::SetCenter(const Vec3f& center)
{
	m_Params.center = center;
	RecalcTransform();
}

S_API const Vec3f& CSphereHelper::GetCenter() const
{
	return m_Params.center;
}

S_API void CSphereHelper::SetRadius(float radius)
{
	m_Params.radius = radius;
	RecalcTransform();
}

S_API float CSphereHelper::GetRadius() const
{
	return m_Params.radius;
}










/*CHelperRenderObject::CHelperRenderObject()
	: m_bAABBInvalid(true),
	m_pRenderer(0),
	m_bReleaseNextFrame(false)
{
}

CHelperRenderObject::~CHelperRenderObject()
{	
	Clear();
}

S_API void CHelperRenderObject::Clear()
{
	if (IS_VALID_PTR(m_pRenderer) && IS_VALID_PTR(m_pRenderer->GetRenderer()) && m_RenderDesc.pSubsets)
	{
		IResourcePool* pResourcePool = m_pRenderer->GetRenderer()->GetResourcePool();
		if (IS_VALID_PTR(pResourcePool))
		{
			SDrawCallDesc& dcd = m_RenderDesc.pSubsets[0].drawCallDesc;
			if (IS_VALID_PTR(dcd.pVertexBuffer)) pResourcePool->RemoveVertexBuffer(&dcd.pVertexBuffer);
			if (IS_VALID_PTR(dcd.pIndexBuffer)) pResourcePool->RemoveIndexBuffer(&dcd.pIndexBuffer);
		}
	}

	m_RenderDesc.Clear();
	m_pRenderer = 0;
	m_bAABBInvalid = true;
}

S_API void CHelperRenderObject::Init(const SHelperInfo& info)
{
	assert(m_pRenderer);
	assert(m_pRenderer->GetRenderer());

	IResourcePool* pRes = m_pRenderer->GetRenderer()->GetResourcePool();
	assert(pRes);

	m_RenderDesc.Clear();
	m_RenderDesc.pSubsets = new SRenderSubset[1];
	m_RenderDesc.nSubsets = 1;
	m_RenderDesc.renderPipeline = eRENDER_FORWARD;
	
	SDrawCallDesc& dcd = m_RenderDesc.pSubsets[0].drawCallDesc;
	dcd.primitiveType = (info.lines ? PRIMITIVE_TYPE_LINES : PRIMITIVE_TYPE_TRIANGLELIST);

	pRes->AddVertexBuffer(&dcd.pVertexBuffer);
	dcd.pVertexBuffer->Initialize(m_pRenderer->GetRenderer(), eVBUSAGE_STATIC, info.pVertices, info.numVertices);
	dcd.iStartVBIndex = 0;
	dcd.iEndVBIndex = info.numVertices - 1;

	if (!info.lines)
	{
		pRes->AddIndexBuffer(&dcd.pIndexBuffer);
		dcd.pIndexBuffer->Initialize(m_pRenderer->GetRenderer(), eIBUSAGE_STATIC, info.numIndices, info.pIndices);
		dcd.iStartIBIndex = 0;
		dcd.iEndIBIndex = info.numIndices - 1;
	}


	SShaderResources& srs = m_RenderDesc.pSubsets[0].shaderResources;
	srs.diffuse = info.color;
	srs.emissive = info.color;
	srs.illumModel = eILLUM_HELPER;

	m_RenderDesc.bDepthStencilEnable = info.depthTestEnable;

	m_bAABBInvalid = true;
}

S_API void CHelperRenderObject::SetColor(const SColor& color)
{
	if (IS_VALID_PTR(m_RenderDesc.pSubsets) && m_RenderDesc.nSubsets > 0)
	{
		SShaderResources& srs = m_RenderDesc.pSubsets[0].shaderResources;
		srs.diffuse = color.ToFloat3();
		srs.emissive = srs.diffuse;
	}
}

S_API void CHelperRenderObject::SetRenderer(I3DEngine* p3DEngine)
{
	m_pRenderer = p3DEngine;
}

S_API AABB CHelperRenderObject::GetAABB()
{
	if (m_bAABBInvalid)
	{		
		m_bAABBInvalid = false;
		m_AABB.Reset();
		if (IS_VALID_PTR(m_RenderDesc.pSubsets) && m_RenderDesc.nSubsets > 0)
		{
			IVertexBuffer* pVB = m_RenderDesc.pSubsets[0].drawCallDesc.pVertexBuffer;
			if (IS_VALID_PTR(pVB))
			{
				for (unsigned long iVtx = 0; iVtx < pVB->GetVertexCount(); ++iVtx)
				{
					SVertex* pVtx = pVB->GetVertex(iVtx);
					m_AABB.AddPoint(Vec3f(pVtx->x, pVtx->y, pVtx->z));
				}
			}
		}
	}

	return m_AABB;
}

S_API IVertexBuffer* CHelperRenderObject::GetVertexBuffer()
{
	if (IS_VALID_PTR(m_RenderDesc.pSubsets) && m_RenderDesc.nSubsets > 0)
		return m_RenderDesc.pSubsets[0].drawCallDesc.pVertexBuffer;
	else
		return 0;
}
S_API IIndexBuffer* CHelperRenderObject::GetIndexBuffer(unsigned int subset)
{
	if (IS_VALID_PTR(m_RenderDesc.pSubsets) && m_RenderDesc.nSubsets > subset)
		return m_RenderDesc.pSubsets[subset].drawCallDesc.pIndexBuffer;
	else
		return 0;
}

S_API SRenderDesc* CHelperRenderObject::GetRenderDesc()
{
	return &m_RenderDesc;
}

S_API void CHelperRenderObject::SetTransform(const SMatrix& transform)
{
	m_RenderDesc.transform = transform;
}

S_API void CHelperRenderObject::SetCustomViewProjMatrix(const SMatrix* viewProj)
{
	if (IS_VALID_PTR(viewProj))
	{
		m_RenderDesc.bCustomViewProjMtx = true;
		m_RenderDesc.viewProjMtx = *viewProj;
	}
	else
	{
		m_RenderDesc.bCustomViewProjMtx = false;
	}
}

S_API void CHelperRenderObject::Show(bool show)
{
	//TODO: Store a m_bVisible variable on our own, which will be read by the Renderer before executing Render(renderDesc)

	// Hide all subsets
	if (m_RenderDesc.pSubsets && m_RenderDesc.nSubsets > 0)
	{
		for (unsigned int i = 0; i < m_RenderDesc.nSubsets; ++i)
		{
			SRenderSubset& subset = m_RenderDesc.pSubsets[i];

			// do not show invalid subsets
			if (IS_VALID_PTR(subset.drawCallDesc.pVertexBuffer) && IS_VALID_PTR(subset.drawCallDesc.pIndexBuffer))
				subset.render = show;
			else
				subset.render = false;
		}
	}
}

S_API void CHelperRenderObject::Update()
{
}

S_API void CHelperRenderObject::OnRelease()
{
	m_RenderDesc.Clear();
}

S_API void CHelperRenderObject::ReleaseNextFrame()
{
	m_bReleaseNextFrame = true;
}

S_API bool CHelperRenderObject::IsGarbage() const
{
	return m_bReleaseNextFrame;
}*/

SP_NMSPACE_END