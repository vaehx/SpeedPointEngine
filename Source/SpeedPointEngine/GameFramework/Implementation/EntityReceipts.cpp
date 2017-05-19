//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "..\EntityReceipts.h"
#include "SpeedPointEngine.h"
#include <EntitySystem\IEntity.h>
#include <Physics\IPhysics.h>
#include <3DEngine\I3DEngine.h>

SP_NMSPACE_BEG

namespace EntityReceipts
{

	// ----------------------------------------------------------------------------------------

	S_API bool Physical::Apply(IEntity* entity)
	{
		if (!entity)
			return false;





		// TODO:

		//		- Merge "Game Facilities" and "Framework" into "GameFramework":
		//			-> Move Receipt implementations to GameFramework. Let's us use SpeedPointEnv there.

		//		- Create "EntitySystem" project including Scene and Entity
		//			-> Move Object loading/bootstrapping + World File into GameFramework






		IPhysics* pPhysics = SpeedPointEnv::GetPhysics();
		PhysObject* physical = entity->AddComponent(pPhysics->CreatePhysObject());
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
