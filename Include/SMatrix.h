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
			_11( 1 ), _12( 0 ), _13( 0 ), _14( 0 ),
			_21( 0 ), _22( 1 ), _23( 0 ), _24( 0 ),
			_31( 0 ), _32( 0 ), _33( 1 ), _34( 0 ),
			_41( 0 ), _42( 0 ), _43( 0 ), _44( 1 ) {};

		SMatrix( const SMatrix& o ) :
			_11( o._11 ), _12( o._12 ), _13( o._13 ), _14( o._14 ),
			_21( o._21 ), _22( o._22 ), _23( o._23 ), _24( o._24 ),
			_31( o._31 ), _32( o._32 ), _33( o._33 ), _34( o._34 ),
			_41( o._41 ), _42( o._42 ), _43( o._43 ), _44( o._44 ) {};

		SMatrix( const D3DXMATRIX& o )
		{
			m[0][0] = o._11; m[0][1] = o._12; m[0][2] = o._13; m[0][3] = o._14;
			m[1][0] = o._21; m[1][1] = o._22; m[1][2] = o._23; m[1][3] = o._24;
			m[2][0] = o._31; m[2][1] = o._32; m[2][2] = o._33; m[2][3] = o._34;
			m[3][0] = o._41; m[3][1] = o._42; m[3][2] = o._43; m[3][3] = o._44;
		};

		SMatrix& operator = ( const SMatrix& o )
		{
			m[0][0] = o._11; m[0][1] = o._12; m[0][2] = o._13; m[0][3] = o._14;
			m[1][0] = o._21; m[1][1] = o._22; m[1][2] = o._23; m[1][3] = o._24;
			m[2][0] = o._31; m[2][1] = o._32; m[2][2] = o._33; m[2][3] = o._34;
			m[3][0] = o._41; m[3][1] = o._42; m[3][2] = o._43; m[3][3] = o._44;

			return *this;
		}

		operator D3DXMATRIX() const
		{
			D3DXMATRIX mtxOut;
			mtxOut._11 = _11; mtxOut._12 = _12; mtxOut._13 = _13; mtxOut._14 = _14;
			mtxOut._21 = _12; mtxOut._22 = _22; mtxOut._23 = _23; mtxOut._24 = _24;
			mtxOut._31 = _13; mtxOut._32 = _32; mtxOut._33 = _33; mtxOut._34 = _34;
			mtxOut._41 = _14; mtxOut._42 = _42; mtxOut._43 = _43; mtxOut._44 = _44;
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