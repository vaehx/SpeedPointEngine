///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "..\PhysObject.h"
#include "PhysDebug.h"
#include <Windows.h>

SP_NMSPACE_BEG

using namespace geo;

S_API PhysObject::PhysObject()
	: m_pShape(0),
	m_pTransformedShape(0),
	m_Behavior(ePHYSOBJ_BEHAVIOR_RIGID_BODY),
	m_bTrash(false),
	m_pHelper(0),
	m_bHelperShown(false)
{
	m_State.M = 0.0f;
	m_State.Minv = 0.0f;
	m_State.damping = 0.95f;
	m_State.gravity = true;
	m_State.livingMoves = false;

	m_Scale = Vec3f(1.0f, 1.0f, 1.0f);
}

S_API PhysObject::~PhysObject()
{
	Clear();
}

S_API void PhysObject::Clear()
{
	if (m_pShape) delete m_pShape;
	m_pShape = 0;

	if (m_pTransformedShape) delete m_pTransformedShape;
	m_pTransformedShape = 0;

	m_bHelperShown = false;
	if (m_pHelper)
	{
		m_pHelper->Clear();
		delete m_pHelper;
		m_pHelper = 0;
	}
}

S_API void PhysObject::SetBehavior(EPhysObjectBehavior behavior)
{
	switch (m_Behavior = behavior)
	{
	case ePHYSOBJ_BEHAVIOR_RIGID_BODY:
		break;

	case ePHYSOBJ_BEHAVIOR_STATIC:
		m_State.Minv = 0.0f;
		m_State.Iinv = Mat33(0);
		m_State.gravity = false;
		break;

	case ePHYSOBJ_BEHAVIOR_LIVING:
		m_State.L = Vec3f(0);
		m_State.w = Vec3f(0);
		break;
	}
}

S_API const shape* PhysObject::GetTransformedCollisionShape() const
{
	if (!m_pShape)
		return 0;
	else if (m_pShape->GetType() == eSHAPE_MESH)
		return m_pShape;
	else
		return m_pTransformedShape;
}

S_API Mat33 Star(const Vec3f& v)
{
	return Mat33(
		0, -v.z, v.y,
		v.z, 0, -v.x,
		-v.y, v.x, 0
	);
}

S_API void PhysObject::Update(float fTime)
{
	if (m_State.Minv > 0)
	{
		Mat33 R = m_State.rotation.ToRotationMatrix33();
		Mat33 Ibodyinv = m_State.Ibodyinv;
		//float density = m_State.M / m_State.V;
		//Ibodyinv._11 *= density; Ibodyinv._22 *= density; Ibodyinv._33 *= density;
		m_State.Iinv = R * Ibodyinv * R.Transposed();

		m_State.v = m_State.Minv * m_State.P;
		m_State.w = m_State.Iinv * m_State.L;
	}


	if (m_Behavior == ePHYSOBJ_BEHAVIOR_LIVING)
		m_State.w = Vec3f(0);

	m_State.pos += m_State.v * fTime;

	float wln = m_State.w.Length();
	if (wln < FLT_EPSILON)
		m_State.rotation = Quat(cosf(wln * fTime * 0.5f), m_State.w * (fTime * 0.5f)) * m_State.rotation;
	else
		m_State.rotation = Quat::FromAxisAngle(m_State.w / wln, wln * fTime) * m_State.rotation;

	// TODO: Accumulate forces
	Vec3f force;
	if (m_State.gravity)
		force += Vec3f(0, -9.81f, 0) * m_State.M;

	m_State.P += force * fTime;
	m_State.P *= powf(m_State.damping, fTime);

	// TODO: Accumulate torque (rotational forces)
	Vec3f torque = Vec3f(0);

	m_State.L += torque * fTime;

	if (m_Behavior == ePHYSOBJ_BEHAVIOR_LIVING)
		m_State.L = Vec3f(0);


	// Will be set after PhysObject::Update()
	m_State.livingOnGround = false;


	if (m_pShape)
	{
		STransformationDesc transform;
		transform.translation = Mat44::MakeTranslationMatrix(m_State.pos - m_State.centerOfMass);
		transform.scale = Mat44::MakeScaleMatrix(m_Scale);
		
		if (m_Behavior == ePHYSOBJ_BEHAVIOR_LIVING)
			transform.rotation = Mat44::Identity;
		else
			transform.rotation = m_State.rotation.ToRotationMatrix();
		
		Mat44 mtx = transform.BuildTRS();

		// Transform collision shape
		switch (m_pShape->GetType())
		{
		case eSHAPE_SPHERE:
			{
				sphere* psphere = (sphere*)m_pShape;
				sphere* ptransformed = (sphere*)m_pTransformedShape;
				ptransformed->c = (mtx * Vec4f(psphere->c, 1.0f)).xyz();
				ptransformed->r = psphere->r;
				break;
			}
		case eSHAPE_CYLINDER:
			{
				cylinder* pcyl = (cylinder*)m_pShape;
				cylinder* ptransformed = (cylinder*)m_pTransformedShape;
				ptransformed->p[0] = (mtx * Vec4f(pcyl->p[0], 1.0f)).xyz();
				ptransformed->p[1] = (mtx * Vec4f(pcyl->p[1], 1.0f)).xyz();
				ptransformed->r = pcyl->r;
				break;
			}
		case eSHAPE_CAPSULE:
			{
				capsule* pcapsule = (capsule*)m_pShape;
				capsule* ptransformed = (capsule*)m_pTransformedShape;
				Vec3f bottom = pcapsule->c - pcapsule->hh * pcapsule->axis, top = pcapsule->c + pcapsule->hh * pcapsule->axis;
				bottom = (mtx * Vec4f(bottom, 1.0f)).xyz();
				top = (mtx * Vec4f(top, 1.0f)).xyz();
				ptransformed->c = (bottom + top) * 0.5f;
				ptransformed->hh = (top - bottom).Length();
				ptransformed->axis = (top - bottom) / ptransformed->hh;
				ptransformed->hh *= 0.5f;
				ptransformed->r = pcapsule->r;
				break;
			}
		case eSHAPE_BOX:
			{
				box* pbox = (box*)m_pShape;
				box* ptransformed = (box*)m_pTransformedShape;
				ptransformed->c = pbox->c + m_State.pos;
				for (int i = 0; i < 3; ++i)
					ptransformed->axis[i] = m_State.rotation * pbox->axis[i];
				break;
			}
		case eSHAPE_MESH:
			{
				mesh* pmesh = (mesh*)m_pShape;
				m_pTransformedShape = 0;
				pmesh->transform = mtx;
				break;
			}
		case eSHAPE_PLANE:
			{
				plane* pplane = (plane*)m_pShape;
				plane* ptransformed = (plane*)m_pTransformedShape;
				ptransformed->n = pplane->n;
				ptransformed->d = pplane->d;
				break;
			}
		default:
			break;
		}

		// Update AABB
		m_AABB.Reset();
		OBB obb = (m_pTransformedShape ? m_pTransformedShape->GetBoundBox() : m_pShape->GetBoundBox());

		for (int x = -1; x < 2; x += 2)
			for (int y = -1; y < 2; y += 2)
				for (int z = -1; z < 2; z += 2)
				{
					m_AABB.AddPoint(obb.center 
						+ (float)x * obb.dimensions[0] * obb.directions[0]
						+ (float)y * obb.dimensions[1] * obb.directions[1]
						+ (float)z * obb.dimensions[2] * obb.directions[2]);
				}

		UpdateHelper();
	}
}

S_API void PhysObject::UpdateHelper()
{
	if (m_pHelper && m_pHelper->IsShown())
		m_pHelper->UpdateFromShape(m_pTransformedShape ? m_pTransformedShape : m_pShape);
}



Vec3f CalculateClosestPointOnShape(const shape* pshape, const Vec3f& p)
{
	Vec3f cp = p;
	switch (pshape->GetType())
	{
	case eSHAPE_SPHERE:
		{
			sphere* psphere = (sphere*)pshape;
			cp = psphere->c + (p - psphere->c).Normalized() * psphere->r;
			break;
		}
	case eSHAPE_CAPSULE:
		{
			capsule* pcapsule = (capsule*)pshape;
			float t = min(max(Vec3Dot(p - pcapsule->c, pcapsule->axis), -pcapsule->hh), pcapsule->hh);
			cp = pcapsule->c + t * pcapsule->axis;
			break;
		}

		// TODO ...

	default:
		break;
	}

	return cp;
}


// pinters->n is supposed to be on the terrain
S_API void PhysObject::ResolveLivingTerrainContact(const PhysTerrain* pterrain, const SIntersection* pinters, float fTime)
{
	const float LIVING_RESTING_TOLERANCE = 0.5f;
	const float GROUND_FRICTION = 0.9f; // 1.0f = no friction, 0 = 100% friction

	float vrel_ln = Vec3Dot(pinters->n, m_State.v);
	if (vrel_ln <= LIVING_RESTING_TOLERANCE)
	{
		m_State.P += -vrel_ln * pinters->n * m_State.M;
		m_State.v = m_State.P * m_State.Minv;

		// Friction when living entity is not actively moving forward
		if (!m_State.livingMoves)
			m_State.P += -m_State.P * GROUND_FRICTION;
	}

	if (vrel_ln > 0 && vrel_ln < 0.9f) // moving forward, slightly down hill
	{
		m_State.P -= pinters->n * vrel_ln * 0.95f;
	}
	
	// Resolve interpenetration
	m_State.pos -= 0.95f * (pinters->n * pinters->dist);

	m_State.livingOnGround = true;
}

float __sqr(float f) { return f * f; }
float __cube(float f) { return f * f * f; }

S_API void PhysObject::RecalculateInertia()
{
	if (!m_pShape)
		return;

	Mat33 Ibody;
	float V;
	Vec3f centerOfMass;
	switch (m_pShape->GetType())
	{
	case eSHAPE_BOX:
		{
			box* pbox = (box*)m_pShape;
			float w = pbox->dim[0] * 2.0f;
			float h = pbox->dim[1] * 2.0f;
			float d = pbox->dim[2] * 2.0f;
			V = w * h * d;
			float c = V / 12.0f;
			Ibody._11 = c * (h * h + d * d);
			Ibody._22 = c * (w * w + d * d);
			Ibody._33 = c * (w * w + h * h);
			centerOfMass = pbox->c;
			break;
		}
	case eSHAPE_SPHERE:
		{
			sphere* psphere = (sphere*)m_pShape;
			V = (4.0f / 3.0f) * SP_PI * psphere->r * psphere->r;
			Ibody._11 = Ibody._22 = Ibody._33 = 0.4f * V * psphere->r * psphere->r;
			centerOfMass = psphere->c;
			break;
		}
	case eSHAPE_CYLINDER:
		{
			cylinder* pcyl = (cylinder*)m_pShape;
			float h = (pcyl->p[1] - pcyl->p[2]).Length();
			V = SP_PI * pcyl->r * pcyl->r * h;
			Ibody._11 = Ibody._22 = (1.0f / 12.0f) * V * (3.0f * pcyl->r * pcyl->r + h * h);
			Ibody._33 = 0.5f * V * pcyl->r * pcyl->r;
			centerOfMass = (pcyl->p[0] + pcyl->p[1]) * 0.5f;
			break;
		}
	case eSHAPE_CAPSULE:
		{
			capsule* pcapsule = (capsule*)m_pShape;
			float h = 2.0f * pcapsule->hh;
			float hsq = h * h, rsq = pcapsule->r * pcapsule->r;
			float Vcaps = (4.0f / 3.0f) * SP_PI * pcapsule->r * pcapsule->r * pcapsule->r; // 4/3 * pIbody * r^3
			float Vcyl = SP_PI * pcapsule->r * pcapsule->r * h;
			V = Vcyl + Vcaps;
			Ibody._11 = Ibody._33 = Vcyl * (hsq + 3.0f * rsq) / 12.0f + Vcaps * (0.4f * rsq + 0.5f * hsq + 0.375f * h * pcapsule->r);
			Ibody._22 = Vcyl * 0.5f * rsq + Vcaps * 0.4f * rsq;
			centerOfMass = pcapsule->c;
			break;
		}
	case eSHAPE_PLANE:
		{
			V = 1.0f;
			break;
		}
	case eSHAPE_MESH:
		{
			mesh* pmesh = (mesh*)m_pShape;
			if (pmesh->points && pmesh->num_points > 0 && pmesh->indices && pmesh->num_indices > 0)
			{
				// reference point = Vec3f(0)

				const float ONE_SIXTH = 1.0f / 6.0f;
				Vec3f p[3], Xtet;
				float Vtet, Adet;
				Mat33 A, C;
				Mat33 Ccan; // covariance of canonical tetrahedron
				Ccan._11 = Ccan._22 = Ccan._33 = 1.0f / 60.0f;
				Ccan._12 = Ccan._13 = Ccan._21 = Ccan._23 = Ccan._31 = Ccan._32 = 1.0f / 120.0f;

				V = 0;

				for (unsigned int tri = 0; tri < pmesh->num_indices; tri += 3)
				{
					for (int i = 0; i < 3; ++i)
						p[i] = pmesh->points[pmesh->indices[tri + i]];

					A = Mat33::FromColumns(p[0], p[1], p[2]);
					Adet = A.Determinant();

					C += Adet * A * Ccan * A.Transposed();
					Vtet = ONE_SIXTH * Adet; // might be negative
					Xtet = (p[0] + p[1] + p[2] + Vec3f(0)) * 0.25f;
					if (V + Vtet < FLT_EPSILON)
						continue;

					centerOfMass = (centerOfMass * V + Xtet * Vtet) / (V + Vtet);
					V += Vtet;
				}

				// Translate covariance by -centerOfMass
				C += V * (2.0f * Vec3MulT(-centerOfMass, centerOfMass) + Vec3MulT(-centerOfMass, -centerOfMass));

				Ibody = Mat33::Identity * C.Trace() - C;
			}
			else
			{
				V = 1.0f;
			}
			break;
		}
	default:
		break;
	}

	m_State.Ibodyinv		= Ibody.Inverted();
	m_State.V				= V;
	m_State.centerOfMass	= centerOfMass;
}

S_API void PhysObject::SetMeshCollisionShape(const Vec3f* ppoints, u32 npoints, const u32* pindices, u32 nindices, bool octree, u16 maxTreeDepth)
{
	SetCollisionShape<mesh>();
	mesh* pmesh = dynamic_cast<mesh*>(m_pShape);
	if (!pmesh)
		return;

	pmesh->points = 0;
	if ((pmesh->num_points = npoints) > 0 && ppoints)
	{
		pmesh->points = new Vec3f[pmesh->num_points];
		memcpy(pmesh->points, ppoints, sizeof(Vec3f) * pmesh->num_points);
	}

	pmesh->indices = 0;
	if ((pmesh->num_indices = nindices) > 0 && pindices)
	{
		pmesh->indices = new unsigned int[pmesh->num_indices];
		memcpy(pmesh->indices, pindices, sizeof(unsigned int) * pmesh->num_indices);
	}

	pmesh->transform = Mat44::Identity;
	pmesh->root.children = 0;
	pmesh->root.num_children = 0;
	pmesh->root.tris = 0;
	pmesh->root.num_tris = 0;
	
	if (pmesh->points && pmesh->indices)
	{
		LARGE_INTEGER freq, start, end;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&start);

		pmesh->CreateTree(octree, maxTreeDepth);

		QueryPerformanceCounter(&end);
		double elapsed = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
		CLog::Log(S_DEBUG, "Created mesh tree in %.4f milliseconds", elapsed * 1000.0f);
	}

	RecalculateInertia();
}

S_API void PhysObject::Release()
{
	m_bTrash = true;
}

S_API bool PhysObject::IsTrash() const
{
	return m_bTrash;
}

S_API void PhysObject::ShowHelper(bool show)
{
	if (m_bHelperShown == show)
		return;

	m_bHelperShown = show;
	if (!m_pShape)
		return;

	if (m_bHelperShown && !m_pHelper)
	{
		m_pHelper = PhysDebug::CreateHelper();
		m_pHelper->CreateFromShape(m_pShape);
	}

	if (m_pHelper)
	{
		m_pHelper->Show(m_bHelperShown);
		
		// Update once when shown
		if (m_bHelperShown)
			m_pHelper->UpdateFromShape(m_pTransformedShape ? m_pTransformedShape : m_pShape);
	}
}

SP_NMSPACE_END
