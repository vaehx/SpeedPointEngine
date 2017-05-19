#pragma once

#include <Renderer\IIndexBuffer.h>
#include <Renderer\IRenderer.h>
#include <Common\SVertex.h>
#include <Common\SPrerequisites.h>
#include <vector>

using std::vector;

SP_NMSPACE_BEG


#define SP_HELPER_POINT 0
#define SP_HELPER_VECTOR 1
#define SP_HELPER_BOX 2
#define SP_HELPER_SPHERE 3
#define SP_HELPER_DYNAMIC_MESH 4
#define SP_HELPER_PLANE 5
#define SP_HELPER_CYLINDER 6

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct S_API SHelperGeometryDesc
{
	vector<SVertex> vertices;
	vector<SIndex> indices;
	EPrimitiveType topology;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SHelperRenderParams
{
	u32 color; // 0xAARRGGBB
	bool outline;
	bool depthTestEnable;
	bool visible;

	SHelperRenderParams()
		: color(0x00ffffff),
		outline(false),
		depthTestEnable(true),
		visible(true)
	{
	}

	SHelperRenderParams(u32 _color, bool _outline, bool _depthTestEnable)
		: color(_color),
		outline(_outline),
		depthTestEnable(_depthTestEnable)
	{
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class S_API CHelper
{
private:
	bool m_bTrash;

protected:
	Mat44 m_Transform;
	SHelperRenderParams m_RenderParams;

public:
	string _name;

	CHelper()
		: m_bTrash(false)
	{
	}

	virtual ~CHelper() {}

	virtual const Mat44& GetTransform() const
	{
		return m_Transform;
	}

	virtual void SetRenderParams(const SHelperRenderParams& renderParams)
	{
		m_RenderParams = renderParams;
	}

	virtual SHelperRenderParams* GetRenderParams()
	{
		return &m_RenderParams;
	}

	// Quick access to color render param
	// color - 0xAARRGGBB
	virtual void SetColor(u32 color)
	{
		m_RenderParams.color = color;
	}

	virtual void SetColor(const SColor& color)
	{
		m_RenderParams.color = color.ToInt_ARGB();
	}

	virtual void Show(bool show = true) { m_RenderParams.visible = show; }
	virtual void Hide() { Show(false); }

	virtual unsigned int GetTypeId() const = 0;
	
	// Returns a pointer to the renderdesc for the dynamic mesh, or NULL if
	// the helper is not of type eHELPER_DYNAMIC_MESH
	virtual SRenderDesc* GetDynamicMesh()
	{
		return 0;
	}

	// Returns true if this helper should be deleted next frame
	bool IsTrash() const
	{
		return m_bTrash;
	}

	void Release()
	{
		m_bTrash = true;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class S_API CTransformableHelper : public CHelper
{
protected:
	Vec3f m_Pos;
	Quat m_Rotation;
	Vec3f m_Scale;

	virtual void RecalcTransform();

public:
	CTransformableHelper();
	virtual ~CTransformableHelper() {}

	virtual const Mat44& GetTransform() const;
	virtual void SetTransform(const Mat44& transform);

	void SetPos(const Vec3f& pos);
	Vec3f GetPos() const;

	void SetRotation(const Quat& rot);
	Quat GetRotation() const;

	void SetScale(const Vec3f& scale);
	Vec3f GetScale() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Allows vertex buffer to be altered after creation
class S_API CDynamicMeshHelper : public CTransformableHelper
{
public:
	struct Params
	{
		SVertex* pVertices;		
		SIndex* pIndices;
		u32 numVertices;
		u32 numIndices;
		EPrimitiveType topology;

		Params()
			: pVertices(0),
			pIndices(0),
			numVertices(0),
			numIndices(0),
			topology(PRIMITIVE_TYPE_TRIANGLELIST)
		{
		}

		Params(SVertex* _verts, u32 _nVerts, SIndex* _indices, u32 _nIndices, EPrimitiveType _topology)
			: pVertices(_verts),
			numVertices(_nVerts),
			pIndices(_indices),
			numIndices(_nIndices),
			topology(_topology)
		{
		}
	};

private:
	SRenderDesc m_RenderDesc;

	void ClearRenderDesc();

protected:
	virtual void RecalcTransform();

public:
	CDynamicMeshHelper();
	virtual ~CDynamicMeshHelper();

	virtual SRenderDesc* GetDynamicMesh();	
	virtual unsigned int GetTypeId() const { return SP_HELPER_DYNAMIC_MESH; }
	void SetParams(const Params& params);

	IVertexBuffer* GetVertexBuffer() const;

	virtual const Mat44& GetTransform() const;
	virtual void SetTransform(const Mat44& transform);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class S_API CPointHelper : public CHelper
{
public:
	struct Params
	{
		Vec3f pos;

		Params() {}
		Params(const Vec3f& _pos)
			: pos(_pos) {}
	};

private:
	Params m_Params;

public:
	void SetParams(const Params& params);

	static const SHelperGeometryDesc* GetBaseGeometry(bool outline);
	virtual unsigned int GetTypeId() const { return SP_HELPER_POINT; }

	void SetPos(const Vec3f& p);
	const Vec3f& GetPos() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class S_API CBoxHelper : public CHelper
{
public:
	struct Params
	{
		Vec3f center;
		Vec3f dimensions[3]; // orthogonal basis vectors, half-dimensions

		Params() {}
		Params(const Vec3f& _center, const Vec3f& dimX, const Vec3f& dimY, const Vec3f& dimZ)
			: center(_center)
		{
			dimensions[0] = dimX;
			dimensions[1] = dimY;
			dimensions[2] = dimZ;
		}
	};

private:
	Params m_Params;

	void RecalcTransform();

public:
	void SetParams(const Params& params);

	static const SHelperGeometryDesc* GetBaseGeometry(bool outline);
	virtual unsigned int GetTypeId() const { return SP_HELPER_BOX; }

	void SetCenter(const Vec3f& center);
	const Vec3f& GetCenter() const;

	// Half-Dimensions (from center to sides)
	void SetDimensions(const Vec3f& dimX, const Vec3f& dimY, const Vec3f& dimZ);
	const Vec3f& GetDimX() const;
	const Vec3f& GetDimY() const;
	const Vec3f& GetDimZ() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CSphereHelper : public CHelper
{
public:
	struct Params
	{
		Vec3f center;
		float radius;

		Params() {}
		Params(const Vec3f& _center, float _radius)
			: center(_center), radius(_radius) {}
	};

private:
	Params m_Params;

	void RecalcTransform();

public:
	void SetParams(const Params& params);

	static const SHelperGeometryDesc* GetBaseGeometry(bool outline);
	virtual unsigned int GetTypeId() const { return SP_HELPER_SPHERE; }

	void SetCenter(const Vec3f& center);
	const Vec3f& GetCenter() const;

	void SetRadius(float radius);
	float GetRadius() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CVectorHelper : public CHelper
{
public:
	struct Params
	{
		Vec3f p;
		Vec3f v; // should be normalized
		float length;

		Params() : length(1.0f) {}
		Params(const Vec3f& _p, const Vec3f& _v, float _length)
			: p(_p), v(_v), length(_length) {}
	};

private:
	Params m_Params;

public:
	void SetParams(const Params& params);

	static const SHelperGeometryDesc* GetBaseGeometry(bool outline);
	virtual unsigned int GetTypeId() const { return SP_HELPER_VECTOR; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CPlaneHelper : public CHelper
{
public:
	struct Params
	{
		Vec3f n;
		float d;
		float hsize; // length from plane origin to side perpendicularly

		Params() {}
		Params(const Vec3f& _n, float _d, float _hsize) : n(_n), d(_d), hsize(_hsize) {}
		Params(const Vec3f& _p, const Vec3f& _n, float _hsize) : n(_n), hsize(_hsize) { d = Vec3Dot(_p, _n); }
	};

private:
	Params m_Params;

public:
	void SetParams(const Params& params);

	static const SHelperGeometryDesc* GetBaseGeometry(bool outline);
	virtual unsigned int GetTypeId() const { return SP_HELPER_PLANE; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CCylinderHelper : public CHelper
{
public:
	struct Params
	{
		Vec3f p[2]; // bottom, top
		float r;
	};

private:
	Params m_Params;

public:
	void SetParams(const Params& params);

	static const SHelperGeometryDesc* GetBaseGeometry(bool outline);
	virtual unsigned int GetTypeId() const { return SP_HELPER_CYLINDER; }
};

SP_NMSPACE_END
