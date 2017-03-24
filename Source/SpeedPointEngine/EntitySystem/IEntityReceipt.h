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

/*

	An entity receipt defines the properties and the components of the entity.

	Entity receipts may inherit one or more other receipts. This allows receipts
	to extend each other in a very flexible way.

	Entity receipts are designed to be used with scripts and scene serialization.

	The receipt of an entity can only be specified when spawning it. There is no
	altering after spawning possible.

*/

struct S_API IEntityReceipt
{
protected:
	vector<IEntityReceipt*> m_InheritedReceipts;

	template<class EntityReceiptImpl>
	void Inherit()
	{
		IEntityReceipt* receipt = static_cast<IEntityReceipt*>(new EntityReceiptImpl());

		for (auto itReceipt = m_InheritedReceipts.begin(); itReceipt != m_InheritedReceipts.end(); ++itReceipt)
		{
			if (*itReceipt && (*itReceipt)->GetName() == receipt->GetName())
			{
				// Already inherited, do not add a new one
				delete receipt;
				return;
			}
		}

		m_InheritedReceipts.push_back(receipt);
	}

public:
	virtual ~IEntityReceipt()
	{
		for (auto itReceipt = m_InheritedReceipts.begin(); itReceipt != m_InheritedReceipts.end(); ++itReceipt)
			delete *itReceipt;
	}

	// Registers properties and creates the necessary components.
	// This call is propagated recursively to the inherited receipts.
	// Returns true on success, false on failure.
	virtual bool Apply(IEntity* entity)
	{
		bool success = true;
		for (auto itReceipt = m_InheritedReceipts.begin(); itReceipt != m_InheritedReceipts.end(); ++itReceipt)
		{
			IEntityReceipt* inheritedReceipt = *itReceipt;
			if (!inheritedReceipt)
				continue;

			if (!inheritedReceipt->Apply(entity))
				success = false;
		}

		return success;
	}

	virtual const char* GetName() const = 0;

	virtual const vector<IEntityReceipt*>& GetInheritedReceipts() const
	{
		return m_InheritedReceipts;
	}
};

struct S_API IEntityReceiptManager
{
protected:
	virtual void AddReceipt(IEntityReceipt* receipt) = 0;

public:
	virtual ~IEntityReceiptManager() {}

	// Overrides existing receipts with the same name
	template<class EntityReceiptImpl>
	void RegisterReceipt()
	{
		AddReceipt(static_cast<IEntityReceipt*>(new EntityReceiptImpl()));
	}

	virtual IEntityReceipt* GetReceipt(const string& name) const = 0;
	virtual void RemoveReceipt(const string& name) = 0;
	virtual void Clear() = 0;

	// Returns true on success, false if something failed
	virtual bool ApplyReceipt(IEntity* entity, const string& name) const = 0;
};

SP_NMSPACE_END
