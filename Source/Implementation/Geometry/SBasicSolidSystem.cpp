// SpeedPoint Solid System - A collection of solids

#include <Implementation\Geometry\SBasicSolidSystem.h>

namespace SpeedPoint
{
	// **********************************************************************************
	
	S_API SResult SBasicSolidSystem::Initialize( SpeedPointEngine* engine )
	{
		if( engine == NULL ) return S_ERROR;

		pEngine = engine;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SP_ID SBasicSolidSystem::AddSolid( void )
	{
		SP_ID id;
		plSolids.AddItem( SBasicSolid(), &id );

		return id;
	}

	// **********************************************************************************

	S_API SSolid* SBasicSolidSystem::GetSolid( SP_ID id )
	{
		SBasicSolid* pBasicSolid = plSolids.GetItemByUID( id );
		
		return (SSolid*)pBasicSolid;
	}

	// **********************************************************************************

	S_API SResult SBasicSolidSystem::Clear( void )
	{
		for( UINT iSolid = 0; iSolid < plSolids.GetSize(); iSolid++ )
		{
			SBasicSolid* pBasicSolid = plSolids.GetItemByIndirectionIndex( iSolid );
			if( pBasicSolid )			
				pBasicSolid->Clear();							
		}

		plSolids.Clear();
		plSolids.ResetCounts();
		
		return S_SUCCESS;
	}

}