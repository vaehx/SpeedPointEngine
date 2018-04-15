#include "..\CHelper.h"
#include "C3DEngine.h"
#include <Renderer\IResourcePool.h>
#include <Renderer\IVertexBuffer.h>
#include <Renderer\IIndexBuffer.h>
#include <Common\SVertex.h>
#include <Common\SColor.h>

SP_NMSPACE_BEG

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API CTransformableHelper::CTransformableHelper()
	: m_Scale(1.0f)
{
	RecalcTransform();
}

S_API void CTransformableHelper::RecalcTransform()
{
	MakeTransformationTRS(m_Pos, m_Rotation.ToRotationMatrix(), m_Scale, &m_Transform);
}

S_API const Mat44& CTransformableHelper::GetTransform() const
{
	return m_Transform;
}

S_API void CTransformableHelper::SetTransform(const Mat44& transform)
{
	m_Transform = transform;
}

S_API void CTransformableHelper::SetPos(const Vec3f& pos)
{
	m_Pos = pos;
	RecalcTransform();
}

S_API Vec3f CTransformableHelper::GetPos() const
{
	return m_Pos;
}

S_API void CTransformableHelper::SetRotation(const Quat& rot)
{
	m_Rotation = rot;
	RecalcTransform();
}

S_API Quat CTransformableHelper::GetRotation() const
{
	return m_Rotation;
}

S_API void CTransformableHelper::SetScale(const Vec3f& scale)
{
	m_Scale = scale;
	RecalcTransform();
}

S_API Vec3f CTransformableHelper::GetScale() const
{
	return m_Scale;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API CDynamicMeshHelper::CDynamicMeshHelper()
{
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
	IResourcePool* pResources = C3DEngine::Get()->GetRenderer()->GetResourcePool();

	if (IS_VALID_PTR(m_RenderDesc.pSubsets) && !IS_VALID_PTR(pResources))
	{
		SDrawCallDesc* dcd = &m_RenderDesc.pSubsets[0].drawCallDesc;
		SP_SAFE_RELEASE(dcd->pVertexBuffer);
		SP_SAFE_RELEASE(dcd->pIndexBuffer);
	}

	m_RenderDesc.Clear();
}

S_API void CDynamicMeshHelper::SetParams(const Params& params)
{
	IRenderer* pRenderer = C3DEngine::Get()->GetRenderer();
	IResourcePool* pResources = pRenderer->GetResourcePool();
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
			dcd->pIndexBuffer->Initialize(pRenderer, eIBUSAGE_STATIC, S_INDEXBUFFER_32, params.pIndices, params.numIndices);
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
	CTransformableHelper::RecalcTransform();
	m_RenderDesc.transform = m_Transform;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API const SHelperGeometryDesc* CPointHelper::GetBaseGeometry(bool outline)
{
	static SHelperGeometryDesc geom;
	static bool geomInited = false;

	if (!geomInited)
	{
		// Create a little diamond shape
		const float sz = 0.08f;						// length of one side of the diamond
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
		const unsigned int nStripes = 25;
		const unsigned int nRings = 25;



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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CVectorHelper::SetParams(const Params& params)
{
	Mat44 mtxTranslation = Mat44::MakeTranslationMatrix(params.p);
	Mat44 mtxRotation = Quat::FromVectors(Vec3f(0, 1.0f, 0), params.v.Normalized()).ToRotationMatrix();
	Mat44 mtxScale = Mat44::MakeScaleMatrix(Vec3f(1.0f, params.length, 1.0f));

	m_Transform = mtxTranslation * (mtxRotation * mtxScale);
}

const SHelperGeometryDesc* CVectorHelper::GetBaseGeometry(bool outline)
{
	static SHelperGeometryDesc geom;
	static bool geomInited = false;

	if (!geomInited)
	{
		geom.topology = PRIMITIVE_TYPE_LINES;
		Vec3f n = Vec3f(0);
		geom.vertices =
		{
			SVertex(0, 0, 0, n.x, n.y, n.z, 0, 0, 0, 0, 0, 1.0f, 1.0f, 1.0f),
			SVertex(0, 1.0f, 0, n.x, n.y, n.z, 0, 0, 0, 0, 0, 0.05f, 0.05f, 0.05f)
		};

		geomInited = true;
	}

	return &geom;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CLineHelper::SetParams(const Params& params)
{
	Vec3f v = params.p2 - params.p1;
	float length = v.Length();
	v /= length;

	Mat44 mtxTranslation = Mat44::MakeTranslationMatrix(params.p1);
	Mat44 mtxRotation = Quat::FromVectors(Vec3f(0, 1.0f, 0), v).ToRotationMatrix();
	Mat44 mtxScale = Mat44::MakeScaleMatrix(Vec3f(1.0f, length, 1.0f));

	m_Transform = mtxTranslation * (mtxRotation * mtxScale);
}

const SHelperGeometryDesc* CLineHelper::GetBaseGeometry(bool outline)
{
	static SHelperGeometryDesc geom;
	static bool geomInited = false;

	if (!geomInited)
	{
		geom.topology = PRIMITIVE_TYPE_LINES;
		Vec3f n = Vec3f(0);
		geom.vertices =
		{
			SVertex(0, 0, 0, n.x, n.y, n.z, 0, 0, 0, 0, 0, 1.0f, 1.0f, 1.0f),
			SVertex(0, 1.0f, 0, n.x, n.y, n.z, 0, 0, 0, 0, 0, 1.0f, 1.0f, 1.0f)
		};

		geomInited = true;
	}

	return &geom;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CPlaneHelper::SetParams(const Params& params)
{
	STransformationDesc transform;
	transform.scale = Mat44::MakeScaleMatrix(Vec3f(params.hsize, 1.0f, params.hsize));
	transform.translation = Mat44::MakeTranslationMatrix(Vec3f(0, params.d + params.n.Length() - 1.0f, 0));
	transform.rotation = Quat::FromVectors(Vec3f(0, 1.0f, 0), params.n.Normalized()).ToRotationMatrix();

	m_Transform = transform.rotation * (transform.translation * transform.scale);
}

const SHelperGeometryDesc* CPlaneHelper::GetBaseGeometry(bool outline)
{
	static SHelperGeometryDesc geom;
	static bool geomInited = false;

	if (!geomInited)
	{
		geom.topology = PRIMITIVE_TYPE_TRIANGLELIST;
		geom.vertices =
		{
			SVertex(-1.0f, 0, -1.0f, 0, 1.0f, 0, 1.0f, 0, 0),
			SVertex(1.0f, 0, -1.0f, 0, 1.0f, 0, 1.0f, 0, 0),
			SVertex(1.0f, 0, 1.0f, 0, 1.0f, 0, 1.0f, 0, 0),
			SVertex(-1.0f, 0, 1.0f, 0, 1.0f, 0, 1.0f, 0, 0)
		};
		geom.indices =
		{
			0, 1, 2,
			0, 2, 3
		};

		geomInited = true;
	}

	return &geom;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CCylinderHelper::SetParams(const Params& params)
{
	STransformationDesc transform;
	transform.translation = Mat44::MakeTranslationMatrix(params.p[0]);
	transform.rotation = Quat::FromVectors(Vec3f(0, 1.0f, 0), (params.p[1] - params.p[0]).Normalized()).ToRotationMatrix();
	transform.scale = Mat44::MakeScaleMatrix(Vec3f(params.r, (params.p[1] - params.p[0]).Length(), params.r));

	m_Transform = transform.translation * (transform.rotation * transform.scale);
}

const SHelperGeometryDesc* CCylinderHelper::GetBaseGeometry(bool outline)
{
	static SHelperGeometryDesc geom;
	static bool geomInited = false;

	if (!geomInited)
	{
		const Vec3f bottom(0, 0, 0);
		const Vec3f top(0, 1.0f, 0);
		const float radius = 1.0f;
		const unsigned int segments = 35;


		// MakeCylinder()
		// TODO: Texture coordinates

		geom.topology = PRIMITIVE_TYPE_TRIANGLELIST;

		unsigned int numMantleVerts = (segments + 1) * 2;
		unsigned int numCapVerts = 1 + segments; // including middle point
		geom.vertices.resize(numMantleVerts + numCapVerts * 2);

		unsigned int numMantleIndices = segments * 6;
		unsigned int numCapIndices = segments * 3;
		geom.indices.resize(numMantleIndices + numCapIndices * 2);

		geom.vertices[numMantleVerts] = SVertex(bottom.x, bottom.y, bottom.z);
		geom.vertices[numMantleVerts + numCapVerts] = SVertex(top.x, top.y, top.z);

		Vec3f axis = top - bottom;
		Vec3f naxis = axis.Normalized();
		Vec3f perp = ((Vec3f(-naxis.y, -naxis.z, naxis.x) ^ naxis) ^ naxis).Normalized() * radius;

		float dAlpha = (2.0f * SP_PI) / (float)segments;
		float alpha;
		Vec3f pbottom, ptop;
		for (unsigned int seg = 0; seg <= segments; ++seg)
		{
			alpha = dAlpha * seg;
			pbottom = bottom + Quat::FromAxisAngle(naxis, alpha) * perp;
			ptop = pbottom + axis;

			Vec3f n = (pbottom - bottom).Normalized();
			Vec3f t = n ^ naxis;

			unsigned int mantleVtx = seg * 2;
			geom.vertices[mantleVtx] = SVertex(pbottom.x, pbottom.y, pbottom.z, n.x, n.y, n.z, t.x, t.y, t.z);
			geom.vertices[mantleVtx + 1] = SVertex(ptop.x, ptop.y, ptop.z, n.x, n.y, n.z, t.x, t.y, t.z);

			if (seg < segments)
			{
				unsigned int capVtx1 = numMantleVerts + 1 + seg;
				geom.vertices[capVtx1] = SVertex(pbottom.x, pbottom.y, pbottom.z, -naxis.x, -naxis.y, -naxis.z, perp.x, perp.y, perp.z);

				unsigned int capVtx2 = numMantleVerts + numCapVerts + 1 + seg;
				geom.vertices[capVtx2] = SVertex(ptop.x, ptop.y, ptop.z, naxis.x, naxis.y, naxis.z, perp.x, perp.y, perp.z);

				unsigned int i = seg * 6;
				geom.indices[i + 0] = mantleVtx;
				geom.indices[i + 1] = mantleVtx + 1;
				geom.indices[i + 2] = mantleVtx + 1 + 2;
				geom.indices[i + 3] = mantleVtx;
				geom.indices[i + 4] = mantleVtx + 1 + 2;
				geom.indices[i + 5] = mantleVtx + 2;

				i = numMantleIndices + seg * 3;
				geom.indices[i + 0] = capVtx1;
				geom.indices[i + 1] = numMantleVerts + 1;
				geom.indices[i + 2] = (numMantleVerts + 1) + (seg + 1) % segments;

				i = numMantleIndices + numCapIndices + seg * 3;
				geom.indices[i + 0] = capVtx2;
				geom.indices[i + 1] = numMantleVerts + numCapVerts + 1;
				geom.indices[i + 2] = (numMantleVerts + numCapVerts + 1) + (seg + 1) % segments;
			}
		}

		geomInited = true;
	}

	return &geom;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MakeCapsule(SInitialGeometryDesc& geom, const Vec3f& bottom, const Vec3f& top, float radius, unsigned int segments, unsigned int rings)
{
	geom.primitiveType = PRIMITIVE_TYPE_TRIANGLELIST;

	auto subset = geom.GetZeroSubset();

	unsigned int numMantleVerts = (segments + 1) * 2;
	unsigned int numCapVerts = (segments + 1) * (rings + 1);
	geom.nVertices = numMantleVerts + numCapVerts * 2;
	geom.pVertices = new SVertex[geom.nVertices];

	unsigned int numMantleIndices = segments * 6;
	unsigned int numCapIndices = segments * rings * 6;
	subset->nIndices = numMantleIndices + numCapIndices * 2;
	subset->pIndices = new SIndex[subset->nIndices];

	Vec3f axis = top - bottom;
	Vec3f naxis = axis.Normalized();
	Vec3f perp = ((Vec3f(-naxis.y, -naxis.z, naxis.x) ^ naxis) ^ naxis).Normalized() * radius;

	float alpha, dAlpha = (2.0f * SP_PI) / (float)segments;
	float beta, dBeta = (SP_PI * 0.5f) / (float)rings;
	Vec3f pbottom, ptop;
	Vec3f cbottom, ctop;
	for (unsigned int seg = 0; seg <= segments; ++seg)
	{
		alpha = dAlpha * seg;
		pbottom = bottom + Quat::FromAxisAngle(naxis, alpha) * perp;
		ptop = pbottom + axis;

		Vec3f n = (pbottom - bottom).Normalized();
		Vec3f t = (n ^ naxis).Normalized();

		unsigned int mantleVtx = seg * 2;
		geom.pVertices[mantleVtx] = SVertex(pbottom.x, pbottom.y, pbottom.z, n.x, n.y, n.z, t.x, t.y, t.z);
		geom.pVertices[mantleVtx + 1] = SVertex(ptop.x, ptop.y, ptop.z, n.x, n.y, n.z, t.x, t.y, t.z);

		if (seg < segments)
		{
			unsigned int i = seg * 6;
			subset->pIndices[i + 0] = mantleVtx;
			subset->pIndices[i + 1] = mantleVtx + 1;
			subset->pIndices[i + 2] = mantleVtx + 1 + 2;
			subset->pIndices[i + 3] = mantleVtx;
			subset->pIndices[i + 4] = mantleVtx + 1 + 2;
			subset->pIndices[i + 5] = mantleVtx + 2;
		}

		for (unsigned int ring = 0; ring <= rings; ++ring)
		{
			beta = dBeta * ring;
			cbottom = bottom + Quat::FromAxisAngle(t, -beta) * (pbottom - bottom);
			ctop = top + Quat::FromAxisAngle(t, beta) * (ptop - top);

			unsigned int capVtxBottom = numMantleVerts + (seg * (rings + 1)) + ring;
			n = Vec3Normalize(cbottom - bottom);
			geom.pVertices[capVtxBottom] = SVertex(cbottom.x, cbottom.y, cbottom.z, n.x, n.y, n.z, t.x, t.y, t.z);

			unsigned int capVtxTop = numMantleVerts + numCapVerts + (seg * (rings + 1)) + ring;
			n = Vec3Normalize(ctop - top);
			geom.pVertices[capVtxTop] = SVertex(ctop.x, ctop.y, ctop.z, n.x, n.y, n.z, t.x, t.y, t.z);

			if (seg < segments && ring < rings)
			{
				unsigned int i = numMantleIndices + ((seg * rings) + ring) * 6;
				subset->pIndices[i + 0] = capVtxBottom;
				subset->pIndices[i + 1] = capVtxBottom + 1;
				subset->pIndices[i + 2] = capVtxBottom + (rings + 1) + 1;
				subset->pIndices[i + 3] = capVtxBottom;
				subset->pIndices[i + 4] = capVtxBottom + (rings + 1) + 1;
				subset->pIndices[i + 5] = capVtxBottom + (rings + 1);

				i = numMantleIndices + numCapIndices + ((seg * rings) + ring) * 6;
				subset->pIndices[i + 0] = capVtxTop;
				subset->pIndices[i + 1] = capVtxTop + 1;
				subset->pIndices[i + 2] = capVtxTop + (rings + 1) + 1;
				subset->pIndices[i + 3] = capVtxTop;
				subset->pIndices[i + 4] = capVtxTop + (rings + 1) + 1;
				subset->pIndices[i + 5] = capVtxTop + (rings + 1);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CHelper* CreateHelperForShape(const geo::shape* pshape, const SColor& color, bool releaseAfterRender, bool outline)
{
	if (!pshape)
		return 0;

	I3DEngine* p3DEngine = C3DEngine::Get();
	CHelper* pHelper = 0;

	switch (pshape->GetType())
	{
	case geo::eSHAPE_RAY:
		{
			const geo::ray* pray = (const geo::ray*)pshape;
			pHelper = p3DEngine->AddHelper<CVectorHelper>(CVectorHelper::Params(pray->p, pray->v, 50.0f), releaseAfterRender);
			break;
		}
	case geo::eSHAPE_PLANE:
		{
			const geo::plane* pplane = (const geo::plane*)pshape;
			CPlaneHelper::Params params;
			params.n = pplane->n;
			params.d = pplane->d;
			params.hsize = 10.0f;
			pHelper = p3DEngine->AddHelper<CPlaneHelper>(params, releaseAfterRender);
			break;
		}
	case geo::eSHAPE_SPHERE:
		{
			const geo::sphere* psphere = (const geo::sphere*)pshape;
			pHelper = p3DEngine->AddHelper<CSphereHelper>(CSphereHelper::Params(psphere->c, psphere->r), releaseAfterRender);
			break;
		}
	case geo::eSHAPE_CYLINDER:
		{
			const geo::cylinder* pcyl = (const geo::cylinder*)pshape;
			CCylinderHelper::Params params;
			params.p[0] = pcyl->p[0];
			params.p[1] = pcyl->p[1];
			params.r = pcyl->r;
			pHelper = p3DEngine->AddHelper<CCylinderHelper>(params, releaseAfterRender);
			break;
		}
	case geo::eSHAPE_CAPSULE:
		{
			const geo::capsule* pcapsule = (const geo::capsule*)pshape;

			SInitialGeometryDesc geomDesc;
			MakeCapsule(geomDesc, pcapsule->c - pcapsule->hh * pcapsule->axis, pcapsule->c + pcapsule->hh * pcapsule->axis, pcapsule->r, 24, 10);

			CDynamicMeshHelper::Params params;
			params.numVertices = geomDesc.nVertices;
			params.pVertices = geomDesc.pVertices;
			params.numIndices = geomDesc.GetZeroSubset()->nIndices;
			params.pIndices = new SLargeIndex[params.numIndices];
			for (u32 i = 0; i < params.numIndices; ++i)
				params.pIndices[i] = (SLargeIndex)geomDesc.GetZeroSubset()->pIndices[i];
			params.topology = geomDesc.primitiveType;

			pHelper = p3DEngine->AddHelper<CDynamicMeshHelper>(params, releaseAfterRender);

			delete[] geomDesc.pVertices;
			delete[] geomDesc.GetZeroSubset()->pIndices;
			delete[] params.pIndices;
			break;
		}
	case geo::eSHAPE_TRIANGLE:
		{
			const geo::triangle* ptri = (const geo::triangle*)pshape;
			CDynamicMeshHelper::Params params;
			params.topology = PRIMITIVE_TYPE_TRIANGLELIST;
			params.numVertices = 3;
			params.pVertices = new SVertex[params.numVertices];
			params.numIndices = 3;
			params.pIndices = new SLargeIndex[params.numIndices];

			for (int i = 0; i < 3; ++i)
			{
				params.pVertices[i] = SVertex(ptri->p[i].x, ptri->p[i].y, ptri->p[i].z);
				params.pIndices[i] = (SLargeIndex)i;
			}

			pHelper = p3DEngine->AddHelper<CDynamicMeshHelper>(params, releaseAfterRender);
			delete[] params.pVertices;
			delete[] params.pIndices;
			break;
		}
	case geo::eSHAPE_MESH:
		{
			const geo::mesh* pmesh = dynamic_cast<const geo::mesh*>(pshape);
			if (!pmesh || !pmesh->points || !pmesh->indices || pmesh->num_points == 0 || pmesh->num_indices == 0)
				return 0;

			// Flatten out triangles so we have sharp edges
			unsigned int ntris = (pmesh->num_indices - (pmesh->num_indices % 3)) / 3;
			CDynamicMeshHelper::Params params;
			params.topology = PRIMITIVE_TYPE_TRIANGLELIST;
			params.pVertices = new SVertex[params.numVertices = ntris * 3];
			params.pIndices = new SLargeIndex[params.numIndices = ntris * 3];

			Vec3f n;
			for (unsigned int itri = 0; itri < ntris * 3; itri += 3)
			{
				const Vec3f &p1 = pmesh->points[pmesh->indices[itri]];
				const Vec3f &p2 = pmesh->points[pmesh->indices[itri + 1]];
				const Vec3f &p3 = pmesh->points[pmesh->indices[itri + 2]];
				n = ((p2 - p1) ^ (p3 - p1)).Normalized();

				for (unsigned int i = 0; i < 3; ++i)
				{
					const Vec3f& p = pmesh->points[pmesh->indices[itri + i]];
					params.pVertices[itri + i] = SVertex(p.x, p.y, p.z, n.x, n.y, n.z, 0, 0, 0);
					params.pIndices[itri + i] = (SLargeIndex)(itri + i);
				}
			}

			pHelper = C3DEngine::Get()->AddHelper<CDynamicMeshHelper>(params);
			pHelper->SetColor(color);

			delete[] params.pVertices;
			delete[] params.pIndices;
			break;
		}
	case geo::eSHAPE_BOX:
		{
			const geo::box* pbox = (const geo::box*)pshape;
			CBoxHelper::Params params;
			params.center = pbox->c;
			for (int i = 0; i < 3; ++i)
				params.dimensions[i] = pbox->axis[i] * pbox->dim[i];
			pHelper = p3DEngine->AddHelper<CBoxHelper>(params, SHelperRenderParams(color, /*outline*/true, true), releaseAfterRender);
			break;
		}
	case geo::eSHAPE_TERRAIN_MESH:
		{
			const geo::terrain_mesh* pmesh = dynamic_cast<const geo::terrain_mesh*>(pshape);
			if (!pmesh || !pmesh->points || pmesh->num_points == 0)
				return 0;

			CDynamicMeshHelper::Params params;
			params.topology = PRIMITIVE_TYPE_TRIANGLELIST;
			params.pVertices = new SVertex[params.numVertices = pmesh->segmentsPerSide * pmesh->segmentsPerSide * 4];
			params.pIndices = new SLargeIndex[params.numIndices = pmesh->segmentsPerSide * pmesh->segmentsPerSide * 2 * 3];
			
			// Flatten out triangles so we can use flat shading for triangles
			SLargeIndex ivert = 0, iidx = 0;
			for (unsigned int y = 0; y < pmesh->segmentsPerSide; ++y)
				for (unsigned int x = 0; x < pmesh->segmentsPerSide; ++x)
				{
					// tris: [ 0->1->2, 0->2->3 ]
					Vec3f p[] =
					{
						pmesh->points[y * (pmesh->segmentsPerSide + 1) + x],
						pmesh->points[(y + 1) * (pmesh->segmentsPerSide + 1) + x],
						pmesh->points[(y + 1) * (pmesh->segmentsPerSide + 1) + (x + 1)],
						pmesh->points[y * (pmesh->segmentsPerSide + 1) + (x + 1)]
					};

					Vec3f n = Vec3Normalize((p[1] - p[0]) ^ (p[3] - p[0]));
					for (int i = 0; i < 4; ++i)
						params.pVertices[ivert + i] = SVertex(p[i].x, p[i].y, p[i].z, n.x, n.y, n.z, 0, 0, 0);

					for (int itri = 0; itri <= 1; ++itri)
						for (int i = 0; i < 3; ++i)
							params.pIndices[iidx++] = (i == 0 ? ivert : ivert + i + itri);

					ivert += 4;
				}

			pHelper = C3DEngine::Get()->AddHelper<CDynamicMeshHelper>(params);
			pHelper->SetColor(color);

			delete[] params.pVertices;
			delete[] params.pIndices;
			break;
		}
	}

	return pHelper;
}



SP_NMSPACE_END
