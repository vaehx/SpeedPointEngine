//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "..\EntityClasses.h"
#include "SpeedPointEngine.h"
#include <EntitySystem\IEntity.h>
#include <Physics\IPhysics.h>
#include <3DEngine\I3DEngine.h>

SP_NMSPACE_BEG

namespace EntityClasses
{
	// ----------------------------------------------------------------------------------------

	S_API bool Physical::Apply(IEntity* entity)
	{
		if (!entity)
			return false;

		entity->AddComponent(SpeedPointEnv::GetPhysics()->CreatePhysObject());

		return true;
	}


	// ----------------------------------------------------------------------------------------

	S_API bool Renderable::Apply(IEntity* entity)
	{
		if (!entity)
			return false;

		I3DEngine* p3DEngine = SpeedPointEnv::GetEngine()->Get3DEngine();
		CRenderMesh* renderable = entity->AddComponent(p3DEngine->CreateMesh());

		// RenderMesh does not provide a geometry property, so we have to implement it here
		entity->RegisterProperty("geometry", this, &Renderable::GetGeometryFile, &Renderable::SetGeometryFile, renderable);
		return true;
	}

	S_API const string& Renderable::GetGeometryFile(CRenderMesh* mesh) const
	{
		return mesh->GetGeometry()->GetFilePath();
	}

	S_API void Renderable::SetGeometryFile(const string& geomFile, CRenderMesh* mesh) const
	{
		IGeometryManager* pGeometryMgr = SpeedPointEnv::Get3DEngine()->GetGeometryManager();
		IGeometry* pGeom = pGeometryMgr->LoadGeometry(geomFile);
		mesh->SetGeometry(pGeom);
	}


	// ----------------------------------------------------------------------------------------

	S_API RigidBody::RigidBody()
	{
		Inherit<Renderable>();
		Inherit<Physical>();
	}

	S_API bool RigidBody::Apply(IEntity* entity)
	{
		if (!entity)
			return false;

		ApplyInherited(entity);

		return true;
	}
}

SP_NMSPACE_END
