//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "EntityReceipt.h"
#include "PhysicalComponent.h"
#include "RenderableComponent.h"
#include <Abstract\IPhysics.h>

SP_NMSPACE_BEG

S_API void EntityReceiptManager::AddReceipt(IEntityReceipt* receipt)
{
	if (!receipt)
		return;

	m_Receipts[receipt->GetName()] = receipt;
}

S_API IEntityReceipt* EntityReceiptManager::GetReceipt(const string& name) const
{
	auto found = m_Receipts.find(name);
	if (found == m_Receipts.end())
		return 0;
	else
		return found->second;
}

S_API bool EntityReceiptManager::ApplyReceipt(IEntity* entity, const string& name) const
{
	if (!entity)
		return false;

	IEntityReceipt* receipt = GetReceipt(name);
	if (!receipt)
	{
		CLog::Log(S_ERROR, "Failed EntityReceiptManager::ApplyReceipt('%s'): Receipt not found", name.c_str());
		return false;
	}

	return receipt->Apply(entity);
}

S_API void EntityReceiptManager::RemoveReceipt(const string& name)
{
	auto found = m_Receipts.find(name);
	if (found == m_Receipts.end())
		return;

	delete found->second;
	found->second = 0;

	m_Receipts.erase(name);
}

S_API void EntityReceiptManager::Clear()
{
	for (auto itReceipt = m_Receipts.begin(); itReceipt != m_Receipts.end(); ++itReceipt)
	{
		delete itReceipt->second;
		itReceipt->second = 0;
	}

	m_Receipts.clear();
}




//////////////////////////////////////////////////////////////////////////////////////////////////


namespace EntityReceipts
{

	// ----------------------------------------------------------------------------------------

	S_API bool Physical::Apply(IEntity* entity)
	{
		if (!entity)
			return false;

		IPhysics* pPhysics = SpeedPointEnv::GetEngine()->GetPhysics();
		CPhysObject* physical = entity->AddComponent(pPhysics->CreatePhysObject());
		
		entity->RegisterProperty("mass", physical, &CPhysObject::GetMass);
		return true;
	}


	// ----------------------------------------------------------------------------------------

	S_API bool Renderable::Apply(IEntity* entity)
	{
		if (!entity)
			return false;

		I3DEngine* p3DEngine = SpeedPointEnv::GetEngine()->Get3DEngine();
		CRenderMesh* renderable = entity->AddComponent(p3DEngine->CreateMesh());

		// RenderMesh does not provide a geomFile property, so we have to define it here
		entity->RegisterProperty("geomFile", this, &Renderable::GetGeomFile, &Renderable::SetGeomFile, renderable);
		return true;
	}

	S_API const string& Renderable::GetGeomFile(CRenderMesh* mesh) const
	{
		return mesh->GetGeometry()->GetGeomFile();
	}

	S_API void Renderable::SetGeomFile(const string& geomFile, CRenderMesh* mesh) const
	{
		// TODO: As RenderMesh does not provide a geomFile property, we have to load the geometry here and
		//		 update the RenderMesh ourselves
	}


	// ----------------------------------------------------------------------------------------

	S_API RigidBody::RigidBody()
	{
		Inherit<Renderable>();
		Inherit<Physical>();
	}

	S_API bool RigidBody::Apply(IEntity* entity)
	{
		// Apply inherited receipts
		return IEntityReceipt::Apply(entity);
	}
}







SP_NMSPACE_END
