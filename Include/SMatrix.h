// ********************************************************************************************

//	SpeedPoint 4 x 4 Matrix

// ********************************************************************************************

#pragma once
#include "SAPI.h"
#include <d3dx9.h>

namespace SpeedPoint
{
	// SpeedPoint 4x4 Matrix
	struct S_API SMatrix
	{
		union
		{
			struct
			{
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};

			float m[4][4];
		};

		SMatrix() :
			_11( 0 ), _12( 0 ), _13( 0 ), _14( 0 ),
			_21( 0 ), _22( 0 ), _23( 0 ), _24( 0 ),
			_31( 0 ), _32( 0 ), _33( 0 ), _34( 0 ),
			_41( 0 ), _42( 0 ), _43( 0 ), _44( 0 ) {};

		SMatrix( const SMatrix& o ) :
			_11( o._11 ), _12( o._12 ), _13( o._13 ), _14( o._14 ),
			_21( o._21 ), _22( o._22 ), _23( o._13 ), _24( o._24 ),
			_31( o._31 ), _32( o._32 ), _33( o._13 ), _34( o._34 ),
			_41( o._41 ), _42( o._42 ), _43( o._13 ), _44( o._44 ) {};

		SMatrix( const D3DXMATRIX& o ) :
			_11( o._11 ), _12( o._12 ), _13( o._13 ), _14( o._14 ),
			_21( o._21 ), _22( o._22 ), _23( o._13 ), _24( o._24 ),
			_31( o._31 ), _32( o._32 ), _33( o._13 ), _34( o._34 ),
			_41( o._41 ), _42( o._42 ), _43( o._13 ), _44( o._44 ) {};

		D3DXMATRIX operator = ( const SMatrix& b )
		{
			return D3DXMATRIX (
				_11, _12, _13, _14,
				_21, _22, _23, _24,
				_31, _32, _33, _34,
				_41, _42, _43, _44 );
		}

		operator D3DXMATRIX()
		{
			D3DXMATRIX mtxOut;
			mtxOut._11 = _11; mtxOut._12 = _12; mtxOut._13 = _13; mtxOut._14 = _14;
			mtxOut._21 = _11; mtxOut._22 = _22; mtxOut._23 = _23; mtxOut._14 = _24;
			mtxOut._31 = _11; mtxOut._32 = _32; mtxOut._33 = _33; mtxOut._14 = _34;
			mtxOut._41 = _11; mtxOut._42 = _42; mtxOut._43 = _43; mtxOut._14 = _44;
			return mtxOut;
		}

	};	

	static void SMatrixIdentity( SMatrix* pMtx )
	{
		if( pMtx != NULL )
		{
			pMtx->_11 = 1; pMtx->_12 = 0; pMtx->_13 = 0; pMtx->_14 = 0;
			pMtx->_21 = 0; pMtx->_22 = 1; pMtx->_23 = 0; pMtx->_24 = 0;
			pMtx->_31 = 0; pMtx->_32 = 0; pMtx->_33 = 1; pMtx->_34 = 0;
			pMtx->_41 = 0; pMtx->_42 = 0; pMtx->_43 = 0; pMtx->_44 = 1;
		}
	}
}