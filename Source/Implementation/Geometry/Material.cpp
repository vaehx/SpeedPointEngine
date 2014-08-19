//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	Material.cpp
// Created:	8/19/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#include <Implementation\Geometry\Material.h>

SP_NMSPACE_BEG


S_API IMaterialLayer* Material::GetLayer(const MaterialLayerID& id) const
{
	return nullptr;
}

S_API SResult Material::AddLayer(const SMaterialLayerDesc& layerDesc)
{
	return S_SUCCESS;
}

S_API SResult Material::DelLayer(const MaterialLayerID& id)
{
	return S_SUCCESS;
}

S_API void Material::Clear()
{

}


SP_NMSPACE_END