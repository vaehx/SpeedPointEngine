#include "..\PhysObject.h"

SP_NMSPACE_BEG

S_API void CPhysObject::Init(const SPhysObjectParams& params)
{
	SetCollisionShape(params.collisionShape);
}

S_API void CPhysObject::SetCollisionShape(const SGeomShape& shape)
{
	m_CollisionShape = shape;
}

S_API SGeomShape* CPhysObject::GetCollisionShape()
{
	return &m_CollisionShape;
}

S_API AABB CPhysObject::GetAABB()
{

	//TODO

	return AABB();



}

SP_NMSPACE_END
