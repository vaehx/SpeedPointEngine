//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IEntity.h"

SP_NMSPACE_BEG

/*

	An entity class defines the properties and the components of the entity.
	
	Entity classes may inherit one or more other classes. This allows classes
	to extend each other in a very flexible way.

	Entity classes are designed to be used with scripts and scene serialization.

	The class of an entity can only be specified when spawning it. There is no
	altering after spawning possible.

*/

struct S_API IEntityClass
{
	virtual ~IEntityClass() {}

	// Registers properties and creates the necessary components.
	// This call is propagated recursively to the inherited classes.
	virtual void Initialize(IEntity* entity) = 0;

	virtual const string& GetName() const = 0;

	virtual const vector<IEntityClass*>& GetInheritedClasses() const = 0;
};

struct S_API IEntityClassManager
{
	virtual ~IEntityClassManager() {}

	virtual void AddClass(IEntityClass* )
};

SP_NMSPACE_END
