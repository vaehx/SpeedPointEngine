//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "EntityReceipt.h"

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

SP_NMSPACE_END
