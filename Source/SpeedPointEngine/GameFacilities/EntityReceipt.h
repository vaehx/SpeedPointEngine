//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Abstract\IEntityReceipt.h>

SP_NMSPACE_BEG

class S_API EntityReceiptManager : public IEntityReceiptManager
{
private:
	map<string, IEntityReceipt*> m_Receipts;

protected:
	// Receipt will be OWNED by the manager, thus will be deleted on removal
	virtual void AddReceipt(IEntityReceipt* receipt);

public:
	virtual ~EntityReceiptManager()
	{
	}

	virtual IEntityReceipt* GetReceipt(const string& name) const;
	virtual bool ApplyReceipt(IEntity* entity, const string& name) const;
	virtual void RemoveReceipt(const string& name);
	virtual void Clear();
};


namespace EntityReceipts
{

	class S_API Physical : public IEntityReceipt
	{
	public:
		virtual bool Apply(IEntity* entity) const;
		virtual const string& GetName() const
		{
			return "Physical";
		}
	};

	class S_API Renderable : public IEntityReceipt
	{
	public:
		virtual bool Apply(IEntity* entity) const;
		virtual const string& GetName() const
		{
			return "Mesh";
		}
	};

	class S_API RigidBody : public IEntityReceipt
	{
	public:
		RigidBody() {}
		~RigidBody() {}

		virtual bool Apply(IEntity* entity) const;
		virtual const string& GetName() const
		{
			return "RigidBody";
		}
	};

}


SP_NMSPACE_END
