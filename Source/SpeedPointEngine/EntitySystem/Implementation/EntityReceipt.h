//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\IEntityReceipt.h"

SP_NMSPACE_BEG

class S_API CRenderMesh;

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

SP_NMSPACE_END
