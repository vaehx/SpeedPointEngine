#include "EntitySystem.h"

SP_NMSPACE_BEG

S_API void EntitySystem::RegisterComponentFactory(IEntityComponentFactory* pFactory)
{
	if (!pFactory)
		return;

	const string& name = pFactory->GetName();
	if (m_Factories.find(name) != m_Factories.end())
		return;

	m_Factories.insert(std::make_pair(name, pFactory));
}

S_API IEntityComponentFactory* EntitySystem::FindComponentFactory(const string& name) const
{
	auto itFactory = m_Factories.find(name);
	if (itFactory != m_Factories.end())
		return itFactory->second;
	else
		return 0;
}

SP_NMSPACE_END
