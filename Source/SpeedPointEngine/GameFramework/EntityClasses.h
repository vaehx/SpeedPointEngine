//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <EntitySystem\IEntityClass.h>

SP_NMSPACE_BEG

class S_API CRenderMesh;

namespace EntityClasses
{
	class S_API Physical : public IEntityClass
	{
	public:
		virtual bool Apply(IEntity* entity);
		virtual const char* GetName() const
		{
			return "Physical";
		}
	};

	class S_API Renderable : public IEntityClass
	{
	private:
		const string& GetGeometryFile(CRenderMesh* mesh) const;
		void SetGeometryFile(const string& file, CRenderMesh* mesh) const;
	
	public:
		virtual bool Apply(IEntity* entity);
		virtual const char* GetName() const
		{
			return "Mesh";
		}
	};

	class S_API RigidBody : public IEntityClass
	{
	public:
		RigidBody();
		~RigidBody() {}

		virtual bool Apply(IEntity* entity);
		virtual const char* GetName() const
		{
			return "RigidBody";
		}
	};
}


SP_NMSPACE_END
