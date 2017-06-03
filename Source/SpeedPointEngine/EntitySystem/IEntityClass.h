//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Common\SerializationTools.h>

SP_NMSPACE_BEG

struct S_API IEntity;

struct S_API IEntityClass
{
protected:
	vector<IEntityClass*> m_InheritedClasses;

	template<class EntityClassImpl>
	void Inherit()
	{
		IEntityClass* pClass = static_cast<IEntityClass*>(new EntityClassImpl());

		for (auto itClass = m_InheritedClasses.begin(); itClass != m_InheritedClasses.end(); ++itClass)
		{
			if (*itClass && (*itClass)->GetName() == pClass->GetName())
			{
				// Already inherited, do not add a new one
				delete pClass;
				return;
			}
		}

		m_InheritedClasses.push_back(pClass);
	}

	void ApplyInherited(IEntity* pEntity)
	{
		IEntityClass::Apply(pEntity);
	}

public:
	virtual ~IEntityClass()
	{
		for (auto itClass = m_InheritedClasses.begin(); itClass != m_InheritedClasses.end(); ++itClass)
			delete *itClass;
	}

	// Registers properties and creates the necessary components.
	// This call is propagated recursively to the inherited classes.
	// Returns true on success, false on failure.
	virtual bool Apply(IEntity* entity)
	{
		bool success = true;
		for (auto itClass = m_InheritedClasses.begin(); itClass != m_InheritedClasses.end(); ++itClass)
		{
			IEntityClass* inheritedClass = *itClass;
			if (!inheritedClass)
				continue;

			if (!inheritedClass->Apply(entity))
				success = false;
		}

		return success;
	}

	virtual const char* GetName() const = 0;

	virtual const vector<IEntityClass*>& GetInheritedClasss() const
	{
		return m_InheritedClasses;
	}
};

struct S_API IEntityClassManager
{
protected:
	virtual void AddClass(IEntityClass* pClass) = 0;

public:
	virtual ~IEntityClassManager() {}

	// Overrides existing classes with the same name
	template<class EntityClassImpl>
	void RegisterEntityClass()
	{
		AddClass(static_cast<IEntityClass*>(new EntityClassImpl()));
	}

	virtual IEntityClass* GetClass(const string& name) const = 0;
	virtual void RemoveClass(const string& name) = 0;
	virtual void Clear() = 0;
};

SP_NMSPACE_END
