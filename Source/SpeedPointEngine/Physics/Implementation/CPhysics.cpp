#include "CPhysics.h"
#include "PhysDebug.h"

SP_NMSPACE_BEG

using namespace geo;

S_API CPhysics::CPhysics()
	: m_pObjects(0),
	m_bPaused(false),
	m_bHelpersShown(false)
{
}

S_API CPhysics::~CPhysics()
{
}

S_API void CPhysics::SetPhysObjectPool(IComponentPool<PhysObject>* pPool)
{
	assert(pPool);
	m_pObjects = pPool;
}

S_API PhysObject* CPhysics::CreatePhysObject()
{
	if (IS_VALID_PTR(m_pObjects))
	{
		PhysObject* pPhysObject = m_pObjects->Get();
		return pPhysObject;
	}
	else
	{
		return 0;
	}
}

S_API void CPhysics::ClearPhysObjects()
{
	if (IS_VALID_PTR(m_pObjects))
	{
		unsigned int i;
		PhysObject* pObj = m_pObjects->GetFirst(i);
		while (pObj)
		{
			pObj->Release();
			pObj = m_pObjects->GetNext(i);
		}

		m_pObjects->ReleaseAll();
	}
}

const char* GetIntersectionFeatureName(EIntersectionFeature f)
{
	switch (f)
	{
	case eINTERSECTION_FEATURE_BASE_SHAPE: return "BASE_SHAPE";
	case eINTERSECTION_FEATURE_CAP: return "CAP";
	default:
		return "??";
	}
}

S_API void CPhysics::Update(float fTime)
{
	if (!m_pObjects)
		return;

	if (m_bPaused)
		fTime = 0.0f;

	unsigned int iObject = 0;
	PhysObject* pObject = 0;

	// Simulate objects further
	for (pObject = m_pObjects->GetFirst(iObject); pObject; pObject = m_pObjects->GetNext(iObject))
	{
		if (pObject->IsTrash())
		{
			m_pObjects->Release(&pObject);
			continue;
		}

		pObject->OnSimulationPrepare();
		pObject->Update(fTime);

		//PhysDebug::VisualizeBox(pObject->GetTransformedCollisionShape()->GetBoundBox(), SColor::White(), true);
	}

	m_Terrain.Update(fTime);

	const geo::shape* pTerrainShape = m_Terrain.GetTransformedCollisionShape();
	if (pTerrainShape)
		PhysDebug::VisualizeBox(pTerrainShape->GetBoundBox(), SColor::White(), true);

	// Determine pairs of objects that possibly collide
	PhysObject *pobj1, *pobj2;
	m_Colliding.clear();
	for (int i = 0; i < m_pObjects->GetNumObjects(); ++i)
	{
		pobj1 = m_pObjects->GetAt(i);
		for (int j = i + 1; j < m_pObjects->GetNumObjects(); ++j)
		{
			pobj2 = m_pObjects->GetAt(j);
			if (pobj1->GetAABB().Intersects(pobj2->GetAABB()))
				m_Colliding.push_back(std::make_pair(pobj1, pobj2));
		}

		//TODO: Use better bounding box hierarchy for terrain to prevent intersection test for each object
		if (pobj1->GetAABB().Intersects(m_Terrain.GetAABB()))
			m_Colliding.push_back(std::make_pair(pobj1, static_cast<PhysObject*>(&m_Terrain)));
	}

	// Find and resolve actual collisions
	for (auto& collidingPair : m_Colliding)
	{
		PhysObject *pobj1 = collidingPair.first, *pobj2 = collidingPair.second;
		const shape* pshape1 = pobj1->GetTransformedCollisionShape();
		const shape* pshape2 = pobj2->GetTransformedCollisionShape();
		if (!pshape1 || !pshape2)
			continue;

		SIntersection contact;
		if (!_Intersection(pshape1, pshape2, &contact))
			continue;

		if (m_bHelpersShown)
		{
			PhysDebug::VisualizePoint(pobj1->GetState()->pos, SColor::Green(), true);

			PhysDebug::VisualizePoint(contact.p, SColor::Red(), true);
			PhysDebug::VisualizeVector(contact.p, contact.n, SColor::Red(), true);
			PhysDebug::VisualizeVector(contact.p, contact.n * contact.dist, SColor::Yellow(), true);
		}

		SPhysObjectState *A = pobj1->GetState(), *B = pobj2->GetState();

		Vec3f Apvel = A->v + (A->w ^ (contact.p - A->pos));
		Vec3f Bpvel = B->v + (B->w ^ (contact.p - B->pos));
		float vrel_ln = Vec3Dot(contact.n, Bpvel - Apvel);
		if (vrel_ln <= RESTING_TOLERANCE) // not a separating contact
		{
			Vec3f r_a = contact.p - A->pos;
			Vec3f r_b = contact.p - B->pos;

			//~~
			//TODO: Determine restitution coefficient by the combination of the colliding materials.
			float rest_coeff = 0.2f; // 0 = no bounce,  1 = 100% bounce
			//~~

			float D_a = Vec3Dot(contact.n, (A->Iinv * (r_a ^ contact.n)) ^ r_a);
			float D_b = Vec3Dot(contact.n, (B->Iinv * (r_b ^ contact.n)) ^ r_b);

			float j = -(1.0f + rest_coeff) * vrel_ln / (A->Minv + B->Minv + D_a + D_b);
			Vec3f force = j * -contact.n;

			if (fabsf(vrel_ln) <= RESTING_TOLERANCE) // resting or sliding contact
			{
				// Apply normal force
				Vec3f AFnormal, BFnormal;
				A->P += (AFnormal = Vec3Dot(contact.n, Apvel) * contact.n) * fTime;
				B->P += (BFnormal = Vec3Dot(contact.n, Bpvel) * contact.n) * fTime;
			}
			else // colliding contact
			{
				// Apply impulse
				A->P += force;
				B->P -= force;
				A->v = A->P * A->Minv;
				B->v = B->P * B->Minv;
			}	

			// Apply friction
			float frictionFactor = 0.95f;
			Vec3f AFfric = frictionFactor * (((contact.p - Apvel) - Vec3Dot(-Apvel, contact.n) * contact.n) - contact.p);
			Vec3f BFfric = frictionFactor * (((contact.p - Bpvel) - Vec3Dot(-Bpvel, contact.n) * contact.n) - contact.p);

			A->P += AFfric;
			B->P += BFfric;
			A->L += (r_a ^ AFfric);
			B->L += (r_b ^ BFfric);
			A->L *= frictionFactor;
			B->L *= frictionFactor;

			// Apply change in rotation due to contact
			A->L += r_a ^ force;
			B->L -= r_b ^ force;
			A->w = A->Iinv * A->L;
			B->w = B->Iinv * B->L;
		}

		// Resolve interpenetration
		if (contact.dist < 0 && !m_bPaused)
		{
			Vec3f dv = (contact.n * contact.dist) / (A->Minv + B->Minv);
			A->pos += (dv * A->Minv) / 5.0f;
			B->pos -= (dv * B->Minv) / 5.0f;
		}
	}

	for (pObject = m_pObjects->GetFirst(iObject); pObject; pObject = m_pObjects->GetNext(iObject))
		pObject->OnSimulationFinished();
}

S_API void CPhysics::CreateTerrainProxy(const float* heightmap, unsigned int heightmapSz[2], const SPhysTerrainParams& params)
{
	m_Terrain.Create(heightmap, heightmapSz, params);
	ShowHelpers(m_bHelpersShown);
}

S_API void CPhysics::UpdateTerrainProxy(const float* heightmap, unsigned int heightmapSz[2], const AABB& bounds)
{
	m_Terrain.UpdateHeightmap(heightmap, heightmapSz, bounds);
}

S_API void CPhysics::ClearTerrainProxy()
{
	m_Terrain.Clear();
}

S_API void CPhysics::ShowHelpers(bool show)
{
	if (m_bHelpersShown == show)
		return;

	m_bHelpersShown = show;

	unsigned int iobj;
	PhysObject* pobj = m_pObjects->GetFirst(iobj);
	while (pobj)
	{
		pobj->ShowHelper(m_bHelpersShown);
		pobj = m_pObjects->GetNext(iobj);
	}

	m_Terrain.ShowHelper(m_bHelpersShown);
}

SP_NMSPACE_END
