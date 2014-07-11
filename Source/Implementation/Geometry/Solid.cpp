// SpeedPoint Basic Solid

#include <Implementation\Geometry\Solid.h>
#include <SpeedPoint.h>
#include <Abstract\IVertexBuffer.h>
#include <Abstract\IIndexBuffer.h>
#include <Util\SVertex.h>

namespace SpeedPoint
{
	S_API Solid::Solid()
		: m_bInitialized(false),
		m_pEngine(nullptr),
		m_pOctree(nullptr)
	{
	}

	S_API Solid::~Solid()
	{
		// Make sure resources are freed
		Clear();
	}


	// **********************************************************************************
	//		GENERAL



	// **********************************************************************************

	S_API SResult Solid::Initialize( SpeedPointEngine* eng, bool dyn )
	{	
		if( eng == NULL ) return S_ABORTED;
		m_pEngine = eng;

		if( Failure( Clear() ) )
			return m_pEngine->LogReport( S_ABORTED, "Could not clear previously initialized Basic solid!" );		
	
		m_bDynamic = dyn;			

		// Create the vertex- and indexbuffer
		IResourcePool* pResourcePool;
		if( NULL == ( pResourcePool = m_pEngine->GetResourcePool()) )
		{
			return m_pEngine->LogReport( S_ERROR, "Could not get Engine Resource pool while initializing basic solid" );
		}

		// VertexBuffer		
		if( Failure( pResourcePool->AddVertexBuffer( NULL, &m_iVertexBuffer ) ) )
		{
			return m_pEngine->LogReport( S_ERROR, "Could not create Vertex buffer for solid" );
		}		

		if( Failure( pResourcePool->AddIndexBuffer( NULL, &m_iIndexBuffer ) ) )
		{
			return m_pEngine->LogReport( S_ERROR, "Could not create Index Buffer for solid!" );
		}

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult Solid::Clear( void )
	{
		if( m_bInitialized )
		{
			IVertexBuffer* pVertexBuffer = m_pEngine->GetResourcePool()->GetVertexBuffer( m_iVertexBuffer );
			if( pVertexBuffer )
			{
				pVertexBuffer->Clear();
				m_pEngine->GetResourcePool()->RemoveVertexBuffer( m_iVertexBuffer );
				m_iVertexBuffer = SP_ID();
			}

			IIndexBuffer* pIndexBuffer = m_pEngine->GetResourcePool()->GetIndexBuffer( m_iIndexBuffer );
			if( pIndexBuffer )
			{
				pIndexBuffer->Clear();
				m_pEngine->GetResourcePool()->RemoveIndexBuffer( m_iIndexBuffer );
				m_iIndexBuffer = SP_ID();
			}

			if( m_pOctree )
			{
				m_pOctree->Clear();
				delete m_pOctree;
				m_pOctree = NULL;
			}

			m_pEngine = NULL;
		}

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult Solid::SetGeometryData( SVertex* pVertices, UINT nVertices, SIndex* pdwIndices, UINT nIndices )
	{
		if( m_pEngine == NULL ) return S_ABORTED;

		if( pVertices == NULL && pdwIndices == NULL )
			return m_pEngine->LogReport( S_ABORTED, "NullPointers given as geometry data when settings basic solid geometry" );

		
		// Fill Vertices
		IVertexBuffer* pVertexBuffer = m_pEngine->GetResourcePool()->GetVertexBuffer( m_iVertexBuffer );
		
		if( pVertexBuffer == NULL )		
			return m_pEngine->LogReport( S_ERROR, "BasicSolid seems not to be initialized yet (invalid Vertexbuffer)!" );
		
		if( pVertexBuffer->IsInited() && Failure( pVertexBuffer->Clear() ) )			
			return m_pEngine->LogReport( S_ERROR, "Could not clear previously initialized Vertex buffer when setting geometry" );					

		if( Failure( pVertexBuffer->Initialize( nVertices, m_bDynamic, m_pEngine, m_pEngine->GetRenderer() ) ) )
			return m_pEngine->LogReport( S_ERROR, "Could not initilize Vertex Buffer when setting geometry data for basic solid!" );

		if( Failure( pVertexBuffer->Fill( pVertices, nVertices, false ) ) )
			return m_pEngine->LogReport( S_ERROR, "Could not set vertex geometry data for basic solid" );


		// Fill Indices
		IIndexBuffer* pIndexBuffer = m_pEngine->GetResourcePool()->GetIndexBuffer( m_iIndexBuffer );		

		if( pIndexBuffer == NULL )
			return m_pEngine->LogReport( S_ERROR, "BasicSolid seems not to be initialized yet (invalid Indexbuffer)!" );

		if( pIndexBuffer->IsInited() && Failure( pIndexBuffer->Clear() ) )
			return m_pEngine->LogReport( S_ERROR, "Failed clear previously initialized indexbuffer when setting geometry" );





		// TODO: check if using 16 bit oder 32 index buffer!

		if( Failure( pIndexBuffer->Initialize( nIndices, m_bDynamic, m_pEngine, m_pEngine->GetRenderer(), S_INDEXBUFFER_32 ) ) )
			return m_pEngine->LogReport( S_ERROR, "Could not initialize Index Buffer when settings geometry data for basic solid!" );







		if( Failure( pIndexBuffer->Fill( pdwIndices, nIndices, false ) ) )
			return m_pEngine->LogReport( S_ERROR, "Could not set index geometry data for basic solid!" );


		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SP_ID Solid::GetVertexBuffer( void )
	{
		return m_iVertexBuffer;
	}

	// **********************************************************************************

	S_API SP_ID Solid::GetIndexBuffer( void )
	{
		return m_iIndexBuffer;
	}	

	// **********************************************************************************

	S_API SResult Solid::RenderSolid(SpeedPointEngine* m_pEngineReplacement)
	{
		SpeedPointEngine* pFinalEngine = m_pEngine;
		
		if (m_pEngineReplacement != 0)		
			pFinalEngine = m_pEngineReplacement;

		SP_ASSERTR(pFinalEngine, S_ABORTED, "Engine is zero! Not initialized?");
		SP_ASSERTR(pFinalEngine->GetRenderer() && pFinalEngine->GetRenderer()->GetRenderPipeline(), S_ABORTED, "Renderer or Render pipeline of engine is zero!");

		return pFinalEngine->GetRenderer()->GetRenderPipeline()->RenderSolidGeometry((ISolid*)this, false);
	}


	// **********************************************************************************
	//		PRIMITIVE



	// **********************************************************************************
	
	S_API SP_ID Solid::AddPrimitive( const SPrimitive& primitive )
	{
		if( m_pEngine == NULL ) return SP_ID();

		SP_ID iUID;
		SPrimitive* pPrimitive = m_plPrimitives.AddItem( primitive, &iUID );
		
		if( pPrimitive == NULL )
		{
			m_pEngine->LogReport( S_ERROR, "Failed to add Primitive to Basic solid!" );
			return SP_ID();
		}

		return iUID;
	}

	// **********************************************************************************
	
	S_API SPrimitive* Solid::GetPrimitive( SP_ID id )
	{
		return m_plPrimitives.GetItemByUID( id );
	}

	// **********************************************************************************

	S_API SPrimitive* Solid::GetPrimitive( SP_UNIQUE index )
	{
		return m_plPrimitives.GetItemByIndirectionIndex( index );
	}

	// **********************************************************************************

	S_API usint32 Solid::GetPrimitiveCount( void )
	{
		return m_plPrimitives.GetSize();
	}




	// **********************************************************************************

	//		MATERIAL




	// **********************************************************************************
	
	S_API void Solid::SetMaterial( const SMaterial& mat )
	{
		m_matMaterial = mat;
		return;
	}

	// **********************************************************************************

	S_API SMaterial* Solid::GetMaterial( void )
	{
		return &m_matMaterial;
	}



	// **********************************************************************************
	//		BOUND BOX



	// **********************************************************************************

	S_API SBoundBox* Solid::RecalculateBoundBox( void )
	{
		if( m_pEngine == NULL ) return NULL;

		IVertexBuffer* pVertexBuffer = m_pEngine->GetResourcePool()->GetVertexBuffer( m_iVertexBuffer );
		
		if( pVertexBuffer == NULL || pVertexBuffer->GetVertexCount() < 0 ) return NULL;		

		if( pVertexBuffer->GetVertexCount() == 0 )
		{
			m_bbBoundBox.vMin = S_DEFAULT_VEC3;
			m_bbBoundBox.vMax = S_DEFAULT_VEC3;			
			return &m_bbBoundBox;
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
	
		m_bbBoundBox.vMin = cMin;
		m_bbBoundBox.vMax = cMax;
	
		return &m_bbBoundBox;
	}

	// **********************************************************************************

	S_API SBoundBox* Solid::GetBoundBox( void )
	{
		return &m_bbBoundBox;
	}

	// **********************************************************************************

	S_API SBoundBox* Solid::SetBoundBox( const SBoundBox& bb )
	{
		m_bbBoundBox = bb;
		return &m_bbBoundBox;
	}



	// **********************************************************************************
	//		OCTREE


	// **********************************************************************************

	S_API IOctree* Solid::GetOctree( void )
	{
		return m_pOctree;
	}




	// **********************************************************************************
	//		ANIMATIONS


	// **********************************************************************************

	S_API void Solid::SetAnimationBundle( SP_ID iBundle )
	{
		return;
	}

	// **********************************************************************************

	S_API SP_ID Solid::GetAnimationBundle( void )
	{
		return SP_ID();
	}

	// **********************************************************************************

	S_API SResult Solid::RunAnimation( SString cName )
	{
		return S_NOTIMPLEMENTED;
	}

	// **********************************************************************************

	S_API SResult Solid::GetCurrentAnimation( SString* dest )
	{
		return S_NOTIMPLEMENTED;
	}

	// **********************************************************************************

	S_API SResult Solid::TickAnimation( float fFrameDelay )
	{
		return S_NOTIMPLEMENTED;
	}

}