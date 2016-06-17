//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2015, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	Entities.cpp
// Created:	6/24/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#include <Implementation\Geometry\Entity.h>
#include <Implementation\Geometry\RenderableComponent.h>
#include <Abstract\IGameEngine.h>
#include <Abstract\I3DEngine.h>
#include <Abstract\IVertexBuffer.h>

SP_NMSPACE_BEG

S_API CEntity::CEntity(IEntitySystem* pEntitySystem)
	: m_pEntitySystem(pEntitySystem)
{
}

S_API const char* CEntity::GetName() const
{
	return m_Name.c_str();
}

S_API void CEntity::SetName(const char* name)
{
	m_Name = name;
}

S_API IComponent* CEntity::CreateComponent(EComponentType component)
{
	if (!IS_VALID_PTR(m_pEntitySystem))
	{
		CLog::Log(S_ERROR, "Cannot create entity component: m_pEntitySystem invalid!");
		return 0;
	}

	if (!IS_VALID_PTR(m_pComponents[component]))
	{
		switch (component)
		{
		case eCOMPONENT_RENDERABLE:
			m_pComponents[component] = m_pEntitySystem->CreateRenderableComponent();
		}

		if (IS_VALID_PTR(m_pComponents[component]))
			m_pComponents[component]->SetEntity(this);
	}

	return m_pComponents[component];
}

S_API IRenderableComponent* CEntity::CreateRenderable()
{
	return (IRenderableComponent*)CreateComponent(eCOMPONENT_RENDERABLE);
}

// Returns NULL if the component was not created
S_API IComponent* CEntity::GetComponent(EComponentType component) const
{
	return m_pComponents[component];
}

S_API void CEntity::SetComponent(EComponentType type, IComponent* pComponent)
{
	m_pComponents[type] = pComponent;
}

S_API void CEntity::ReleaseComponent(IComponent* pComponent)
{
	for (int i = 0; i < NUM_COMPONENTS; ++i)
	{
		if (m_pComponents[i] == pComponent)
		{
			m_pComponents[i] = 0;
			break;
		}
	}
}


SP_NMSPACE_END
