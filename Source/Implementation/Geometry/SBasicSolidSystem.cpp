// SpeedPoint Solid System - A collection of solids

#include <Implementation\Geometry\SBasicSolidSystem.h>

namespace SpeedPoint
{
	// **********************************************************************************
	
	S_API SResult SBasicSolidSystem::Initialize( SpeedPointEngine* engine )
	{
		if( engine == NULL ) return S_ERROR;

		m_pEngine = engine;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SSpeedPointEngine* SBasicSolidSystem::GetEngine()
	{
		return m_pEngine;
	}

	// **********************************************************************************

	S_API SP_ID SBasicSolidSystem::AddSolid( void )
	{
		SP_ID id;
		m_plSolids.AddItem( SBasicSolid(), &id );

		return id;
	}

	// **********************************************************************************

	S_API ISolid* SBasicSolidSystem::GetSolid( SP_ID id )
	{
		SBasicSolid* pBasicSolid = m_plSolids.GetItemByUID( id );
		
		return (ISolid*)pBasicSolid;
	}

	// **********************************************************************************

	S_API unsigned int SBasicSolidSystem::GetSolidCount()
	{
		return m_plSolids.GetUsageSize();
	}

	// **********************************************************************************

	S_API SResult SBasicSolidSystem::Clear( void )
	{
		for( usint32 iSolid = 0; iSolid < m_plSolids.GetSize(); iSolid++ )
		{
			SBasicSolid* pBasicSolid = m_plSolids.GetItemByIndirectionIndex( iSolid );
			if( pBasicSolid )			
				pBasicSolid->Clear();							
		}

		m_plSolids.Clear();
		m_plSolids.ResetCounts();
		
		return S_SUCCESS;
	}

}