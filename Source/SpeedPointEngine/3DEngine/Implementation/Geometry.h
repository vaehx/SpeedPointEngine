//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2015, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	RawGeometrical.h
// Created:	8/18/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef __rawgeometrical_h__
#define __rawgeometrical_h__

#if _MSC_VER > 1000
#pragma once
#endif

#include "..\IGeometry.h"
#include <Common\SPrerequisites.h>
#include <Common\ChunkedObjectPool.h>
#include <map>

using std::map;

SP_NMSPACE_BEG

struct S_API I3DEngine;

class S_API CGeometry : public IGeometry
{
protected:
	unsigned int m_RefCount;
	IRenderer* m_pRenderer;
	string m_File; // empty if not loaded
	SGeomSubset* m_pSubsets;
	unsigned short m_nSubsets;
	IVertexBuffer* m_pVertexBuffer;
	EPrimitiveType m_PrimitiveType;

private:
	inline static void CalculateInitialNormalsOrTangents(const SInitialGeometryDesc* pInitialGeom);
	void Clear();

public:
	CGeometry();
	virtual ~CGeometry();	

	virtual void Release();
	virtual void AddRef();
	virtual unsigned int GetRefCount() const { return m_RefCount; };

	virtual SResult Init(IRenderer* pRenderer, const SInitialGeometryDesc* pInitialGeom = nullptr);
	virtual const string& GetFilePath() const { return m_File; }
	virtual IRenderer* GetRenderer()
	{
		return m_pRenderer;
	}
	
	virtual SGeomSubset* GetSubsets() { return m_pSubsets; }
	virtual unsigned short GetSubsetCount() const { return m_nSubsets; }
	virtual SGeomSubset* GetSubset(unsigned int index)
	{
		if (index <= m_nSubsets)
			return &m_pSubsets[index];
		else
			return 0;
	}
	virtual IVertexBuffer* GetVertexBuffer() { return m_pVertexBuffer; }
	virtual SVertex* GetVertices();
	virtual SVertex* GetVertex(unsigned long index);
	virtual SIndex* GetIndex(unsigned long index);
	virtual unsigned long GetVertexCount() const;
	virtual unsigned long GetIndexCount() const;

	virtual SResult HandleShutdown()
	{
		Clear();
		return S_SUCCESS;
	}
	virtual string GetShutdownHandlerDesc() const { return "CGeometry"; }

	virtual EPrimitiveType GetPrimitiveType() const
	{
		return m_PrimitiveType;
	}

	virtual SResult CalculateNormalsGeometry(SInitialGeometryDesc& dsc, float fLineLength = 0.1f) const;
	virtual void CalculateBoundBox(AABB& aabb, const Mat44& transform);
};


/////////////////////////////////////////////////////////////////////////////////////////////

class S_API CGeometryManager : public IGeometryManager
{
private:
	ChunkedObjectPool<CGeometry, 20> m_Geom;
	map<string, CGeometry*> m_NameCache;

public:
	virtual ~CGeometryManager();
	virtual IGeometry* LoadGeometry(const string& spmResourcePath);
	virtual IGeometry* CreateGeometry(const SInitialGeometryDesc& desc, const string& name = "");
	virtual void GarbageCollect();
	virtual void Clear();
};



SP_NMSPACE_END
#endif
