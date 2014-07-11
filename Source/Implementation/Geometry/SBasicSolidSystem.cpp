// SpeedPoint Solid System - A collection of solids

#include <Implementation\Geometry\SolidSystem.h>

namespace SpeedPoint
{
	// **********************************************************************************
	
	S_API SResult SolidSystem::Initialize( SpeedPointEngine* engine )
	{
		if( engine == NULL ) return S_ERROR;

		m_pEngine = engine;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SpeedPointEngine* SolidSystem::GetEngine()
	{
		return m_pEngine;
	}

	// **********************************************************************************

	S_API SP_ID SolidSystem::AddSolid( void )
	{
		SP_ID id;
		m_plSolids.AddItem( Solid(), &id );

		return id;
	}

	// **********************************************************************************

	S_API ISolid* SolidSystem::GetSolid( SP_ID id )
	{
		Solid* pBasicSolid = m_plSolids.GetItemByUID( id );
		
		return (ISolid*)pBasicSolid;
	}

	// **********************************************************************************

	S_API unsigned int SolidSystem::GetSolidCount()
	{
		return m_plSolids.GetUsageSize();
	}

	// **********************************************************************************

	S_API SResult SolidSystem::Clear( void )
	{
		for( usint32 iSolid = 0; iSolid < m_plSolids.GetSize(); iSolid++ )
		{
			Solid* pBasicSolid = m_plSolids.GetItemByIndirectionIndex( iSolid );
			if( pBasicSolid )			
				pBasicSolid->Clear();							
		}

		m_plSolids.Clear();
		m_plSolids.ResetCounts();
				
		return S_SUCCESS;
	}

}