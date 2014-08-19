//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	Material.h
// Created:	8/18/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef __material_h__
#define __material_h__

#if _MSC_VER > 1000
#pragma once
#endif


#include <SPrerequisites.h>
#include <Abstract\IGeometrical.h>
#include <map>

using std::map;

SP_NMSPACE_BEG

class S_API MaterialLayer : public IMaterialLayer
{
private:
	SP_ID m_iDiffuseTex;
	SP_ID m_iSpecularTex;
	MaterialLayerID m_ID;

public:
	MaterialLayer()
	{
	}

	MaterialLayer(const SP_ID& diffuseTex, const SP_ID& specTex)
		: m_iDiffuseTex(diffuseTex),
		m_iSpecularTex(specTex)
	{
	}

	MaterialLayer(const MaterialLayer& o)
		: m_iDiffuseTex(o.m_iDiffuseTex),
		m_iSpecularTex(o.m_iSpecularTex)
	{
	}
	
	virtual ~MaterialLayer()
	{
		Clear();
	}

	virtual MaterialLayerID GetLayerID() const { return m_ID; }	
	virtual void Clear()
	{
	}
};



class S_API Material : public IMaterial
{
private:
	MaterialLayerID m_iMaterialLayerIDCounter;
	map<MaterialLayerID, MaterialLayer>* m_pLayers;

public:
	virtual ~Material()
	{
	}

	virtual IMaterialLayer* GetLayer(const MaterialLayerID& id) const;
	virtual SResult AddLayer(const SMaterialLayerDesc& layerDesc);
	virtual SResult DelLayer(const MaterialLayerID& id);
	virtual void Clear();
};


SP_NMSPACE_END
#endif