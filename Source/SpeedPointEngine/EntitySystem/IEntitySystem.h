#pragma once

#include "IEntity.h"

SP_NMSPACE_BEG

struct S_API IEntityComponentFactory
{
	virtual const string& GetName() const = 0;
	virtual IComponent* CreateComponent() const = 0;
};

template<typename ComponentT>
class S_API CEntityComponentFactory : public IEntityComponentFactory
{
private:
	string m_Name;
public:
	CEntityComponentFactory()
	{
		ComponentT c;
		m_Name = c.GetName();
	}

	virtual const string& GetName() const { return m_Name; }
	virtual IComponent* CreateComponent() const
	{
		return static_cast<IComponent*>(new ComponentT());
	}
};

struct S_API IEntitySystem
{
protected:
	virtual void RegisterComponentFactory(IEntityComponentFactory* pFactory) = 0;
	virtual IEntityComponentFactory* FindComponentFactory(const string& name) const = 0;

public:
	template<typename ComponentT>
	void RegisterComponent()
	{
		RegisterComponentFactory(new CEntityComponentFactory<ComponentT>());
	}

	IComponent* CreateComponent(const string& name) const
	{
		IEntityComponentFactory* pFactory = FindComponentFactory(name);
		if (!pFactory)
			return 0;

		return pFactory->CreateComponent();
	}

	template<typename ComponentT>
	ComponentT* CreateComponent() const
	{
		ComponentT c;
		return static_cast<ComponentT*>(CreateComponent(c.GetName()));
	}
};

SP_NMSPACE_END
