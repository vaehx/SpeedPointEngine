// SpeedPoint Basic Solid

#include <Implementation\Geometry\SBasicSolid.h>
#include <SpeedPoint.h>
#include <Abstract\IVertexBuffer.h>
#include <Abstract\IIndexBuffer.h>
#include <Util\SVertex.h>

namespace SpeedPoint
{
	S_API SBasicSolid::~SBasicSolid()
	{
		// Make sure resources are freed
		Clear();
	}


	// **********************************************************************************
	//		GENERAL



	// **********************************************************************************

	S_API SResult SBasicSolid::Initialize( SpeedPointEngine* eng, bool dyn )
	{	
		if( eng == NULL ) return S_ABORTED;
		pEngine = eng;

		if( Failure( Clear() ) )
			return pEngine->LogReport( S_ABORTED, "Could not clear previously initialized Basic solid!" );		
	
		bDynamic = dyn;			

		// Create the vertex- and indexbuffer
		IResourcePool* pResourcePool;
		if( NULL == ( pResourcePool = pEngine->GetResourcePool()) )
		{
			return pEngine->LogReport( S_ERROR, "Could not get Engine Resource pool while initializing basic solid" );
		}

		// VertexBuffer		
		if( Failure( pResourcePool->AddVertexBuffer( NULL, &iVertexBuffer ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Could not create Vertex buffer for solid" );
		}		

		if( Failure( pResourcePool->AddIndexBuffer( NULL, &iIndexBuffer ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Could not create Index Buffer for solid!" );
		}

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SBasicSolid::Clear( void )
	{
		if( bInitialized )
		{
			IVertexBuffer* pVertexBuffer = pEngine->GetResourcePool()->GetVertexBuffer( iVertexBuffer );
			if( pVertexBuffer )
			{
				pVertexBuffer->Clear();
				pEngine->GetResourcePool()->RemoveVertexBuffer( iVertexBuffer );
				iVertexBuffer = SP_ID();
			}

			IIndexBuffer* pIndexBuffer = pEngine->GetResourcePool()->GetIndexBuffer( iIndexBuffer );
			if( pIndexBuffer )
			{
				pIndexBuffer->Clear();
				pEngine->GetResourcePool()->RemoveIndexBuffer( iIndexBuffer );
				iIndexBuffer = SP_ID();
			}

			if( pOctree )
			{
				pOctree->Clear();
				delete pOctree;
				pOctree = NULL;
			}

			pEngine = NULL;
		}

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SBasicSolid::SetGeometryData( SVertex* pVertices, UINT nVertices, DWORD* pdwIndices, UINT nIndices )
	{
		if( pEngine == NULL ) return S_ABORTED;

		if( pVertices == NULL && pdwIndices == NULL )
			return pEngine->LogReport( S_ABORTED, "NullPointers given as geometry data when settings basic solid geometry" );

		
		// Fill Vertices
		IVertexBuffer* pVertexBuffer = pEngine->GetResourcePool()->GetVertexBuffer( iVertexBuffer );
		
		if( pVertexBuffer == NULL )		
			return pEngine->LogReport( S_ERROR, "BasicSolid seems not to be initialized yet (invalid Vertexbuffer)!" );
		
		if( pVertexBuffer->IsInited() && Failure( pVertexBuffer->Clear() ) )			
			return pEngine->LogReport( S_ERROR, "Could not clear previously initialized Vertex buffer when setting geometry" );					

		if( Failure( pVertexBuffer->Initialize( nVertices, bDynamic, pEngine, pEngine->GetRenderer() ) ) )
			return pEngine->LogReport( S_ERROR, "Could not initilize Vertex Buffer when setting geometry data for basic solid!" );

		if( Failure( pVertexBuffer->Fill( pVertices, nVertices, false ) ) )
			return pEngine->LogReport( S_ERROR, "Could not set vertex geometry data for basic solid" );


		// Fill Indices
		IIndexBuffer* pIndexBuffer = pEngine->GetResourcePool()->GetIndexBuffer( iIndexBuffer );		

		if( pIndexBuffer == NULL )
			return pEngine->LogReport( S_ERROR, "BasicSolid seems not to be initialized yet (invalid Indexbuffer)!" );

		if( pIndexBuffer->IsInited() && Failure( pIndexBuffer->Clear() ) )
			return pEngine->LogReport( S_ERROR, "Failed clear previously initialized indexbuffer when setting geometry" );

		if( Failure( pIndexBuffer->Initialize( nIndices, bDynamic, pEngine, pEngine->GetRenderer(), S_INDEXBUFFER_32 ) ) )
			return pEngine->LogReport( S_ERROR, "Could not initialize Index Buffer when settings geometry data for basic solid!" );

		if( Failure( pIndexBuffer->Fill( pdwIndices, nIndices, false ) ) )
			return pEngine->LogReport( S_ERROR, "Could not set index geometry data for basic solid!" );


		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SP_ID SBasicSolid::GetVertexBuffer( void )
	{
		return iVertexBuffer;
	}

	// **********************************************************************************

	S_API SP_ID SBasicSolid::GetIndexBuffer( void )
	{
		return iIndexBuffer;
	}	

	// **********************************************************************************

	S_API SResult SBasicSolid::RenderSolid(SpeedPointEngine* pEngineReplacement)
	{
		SpeedPointEngine* pFinalEngine = pEngine;
		
		if (pEngineReplacement != 0)		
			pFinalEngine = pEngineReplacement;

		SP_ASSERTR(pFinalEngine, S_ABORTED, "Engine is zero! Not initialized?");
		SP_ASSERTR(pFinalEngine->GetRenderer() && pFinalEngine->GetRenderer()->GetRenderPipeline(), S_ABORTED, "Renderer or Render pipeline of engine is zero!");

		return pFinalEngine->GetRenderer()->GetRenderPipeline()->RenderSolidGeometry((ISolid*)this, false);
	}


	// **********************************************************************************
	//		PRIMITIVE



	// **********************************************************************************
	
	S_API SP_ID SBasicSolid::AddPrimitive( const SPrimitive& primitive )
	{
		if( pEngine == NULL ) return SP_ID();

		SP_ID iUID;
		SPrimitive* pPrimitive = plPrimitives.AddItem( primitive, &iUID );
		
		if( pPrimitive == NULL )
		{
			pEngine->LogReport( S_ERROR, "Failed to add Primitive to Basic solid!" );
			return SP_ID();
		}

		return iUID;
	}

	// **********************************************************************************
	
	S_API SPrimitive* SBasicSolid::GetPrimitive( SP_ID id )
	{
		return plPrimitives.GetItemByUID( id );
	}

	// **********************************************************************************

	S_API SPrimitive* SBasicSolid::GetPrimitive( SP_UNIQUE index )
	{
		return plPrimitives.GetItemByIndirectionIndex( index );
	}

	// **********************************************************************************

	S_API usint32 SBasicSolid::GetPrimitiveCount( void )
	{
		return plPrimitives.GetSize();
	}




	// **********************************************************************************

	//		MATERIAL




	// **********************************************************************************
	
	S_API void SBasicSolid::SetMaterial( const SMaterial& mat )
	{
		matMaterial = mat;
		return;
	}

	// **********************************************************************************

	S_API SMaterial* SBasicSolid::GetMaterial( void )
	{
		return &matMaterial;
	}



	// **********************************************************************************
	//		BOUND BOX



	// **********************************************************************************

	S_API SBoundBox* SBasicSolid::RecalculateBoundBox( void )
	{
		if( pEngine == NULL ) return NULL;

		IVertexBuffer* pVertexBuffer = pEngine->GetResourcePool()->GetVertexBuffer( iVertexBuffer );
		
		if( pVertexBuffer == NULL || pVertexBuffer->GetVertexCount() < 0 ) return NULL;		

		if( pVertexBuffer->GetVertexCount() == 0 )
		{
			bbBoundBox.vMin = S_DEFAULT_VEC3;
			bbBoundBox.vMax = S_DEFAULT_VEC3;			
			return &bbBoundBox;
		}
	
		SVector3 cMin = S_DEFAULT_VEC3;
		SVector3 cMax = S_DEFAULT_VEC3;
	
		for( int i = 0; i < pVertexBuffer->GetVertexCount(); i++ )
		{
			if( i == 100 )
				continue;			

			if( pVertexBuffer->GetVertex(i)->x < cMin.x || cMin.x == 0 )
				cMin.x = pVertexBuffer->GetVertex(i)->x;
			if( pVertexBuffer->GetVertex(i)->y < cMin.y || cMin.y == 0 )
				cMin.y = pVertexBuffer->GetVertex(i)->y;
			if( pVertexBuffer->GetVertex(i)->z < cMin.z || cMin.z == 0 )
				cMin.z = pVertexBuffer->GetVertex(i)->z;
	
			if( pVertexBuffer->GetVertex(i)->x > cMax.x || cMax.x == 0 )
				cMax.x = pVertexBuffer->GetVertex(i)->x;
			if( pVertexBuffer->GetVertex(i)->y > cMax.y || cMax.y == 0 )
				cMax.y = pVertexBuffer->GetVertex(i)->y;
			if( pVertexBuffer->GetVertex(i)->z > cMax.z || cMax.z == 0 )
				cMax.z = pVertexBuffer->GetVertex(i)->z;
		}
	
		bbBoundBox.vMin = cMin;
		bbBoundBox.vMax = cMax;
	
		return &bbBoundBox;
	}

	// **********************************************************************************

	S_API SBoundBox* SBasicSolid::GetBoundBox( void )
	{
		return &bbBoundBox;
	}

	// **********************************************************************************

	S_API SBoundBox* SBasicSolid::SetBoundBox( const SBoundBox& bb )
	{
		bbBoundBox = bb;
		return &bbBoundBox;
	}



	// **********************************************************************************
	//		OCTREE


	// **********************************************************************************

	S_API IOctree* SBasicSolid::GetOctree( void )
	{
		return pOctree;
	}




	// **********************************************************************************
	//		ANIMATIONS


	// **********************************************************************************

	S_API void SBasicSolid::SetAnimationBundle( SP_ID iBundle )
	{
		return;
	}

	// **********************************************************************************

	S_API SP_ID SBasicSolid::GetAnimationBundle( void )
	{
		return SP_ID();
	}

	// **********************************************************************************

	S_API SResult SBasicSolid::RunAnimation( SString cName )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SResult SBasicSolid::GetCurrentAnimation( SString* dest )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SResult SBasicSolid::TickAnimation( float fFrameDelay )
	{
		return S_ERROR;
	}

}