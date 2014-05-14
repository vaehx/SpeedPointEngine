// ********************************************************************************************

//	SpeedPoint 4 x 4 Matrix

// ********************************************************************************************

#pragma once
#include "SAPI.h"
#include "SVector4.h"

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

		/*
		SMatrix( const D3DXMATRIX& o )
		{
			m[0][0] = o._11; m[0][1] = o._12; m[0][2] = o._13; m[0][3] = o._14;
			m[1][0] = o._21; m[1][1] = o._22; m[1][2] = o._23; m[1][3] = o._24;
			m[2][0] = o._31; m[2][1] = o._32; m[2][2] = o._33; m[2][3] = o._34;
			m[3][0] = o._41; m[3][1] = o._42; m[3][2] = o._43; m[3][3] = o._44;
		}
		*/

		SMatrix(const SVector4& v1, const SVector4& v2, const SVector4& v3, const SVector4& v4)
		{
			m[0][0] = v1.x; m[0][1] = v1.y; m[0][2] = v1.z; m[0][3] = v1.w;
			m[1][0] = v2.x; m[1][1] = v2.y; m[1][2] = v2.z; m[1][3] = v2.w;
			m[2][0] = v3.x; m[2][1] = v3.y; m[2][2] = v3.z; m[2][3] = v3.w;
			m[3][0] = v4.x; m[3][1] = v4.y; m[3][2] = v4.z; m[3][3] = v4.w;
		}

		SMatrix& operator = ( const SMatrix& o )
		{
			m[0][0] = o._11; m[0][1] = o._12; m[0][2] = o._13; m[0][3] = o._14;
			m[1][0] = o._21; m[1][1] = o._22; m[1][2] = o._23; m[1][3] = o._24;
			m[2][0] = o._31; m[2][1] = o._32; m[2][2] = o._33; m[2][3] = o._34;
			m[3][0] = o._41; m[3][1] = o._42; m[3][2] = o._43; m[3][3] = o._44;

			return *this;
		}

		SMatrix operator * (const SMatrix& b)
		{
			SMatrix a = *this;
			return SMatrix(
				SVector4(a._11*b._11 + a._12*b._21 + a._13*b._31 + a._14*b._41,
					a._11*b._12 + a._12*b._22 + a._13*b._32 + a._14*b._42,
					a._11*b._13 + a._12*b._23 + a._13*b._33 + a._14*b._43,
					a._11*b._14 + a._12*b._24 + a._13*b._34 + a._14*b._44),
				SVector4(a._21*b._11 + a._22*b._21 + a._23*b._31 + a._24*b._41,
					a._21*b._12 + a._22*b._22 + a._23*b._32 + a._24*b._42,
					a._21*b._13 + a._22*b._23 + a._23*b._33 + a._24*b._43,
					a._21*b._14 + a._22*b._24 + a._23*b._34 + a._24*b._44),
				SVector4(a._31*b._11 + a._32*b._21 + a._33*b._31 + a._34*b._41,
					a._31*b._12 + a._32*b._22 + a._33*b._32 + a._34*b._42,
					a._31*b._13 + a._32*b._23 + a._33*b._33 + a._34*b._43,
					a._31*b._14 + a._32*b._24 + a._33*b._34 + a._34*b._44),
				SVector4(a._41*b._11 + a._42*b._21 + a._43*b._31 + a._44*b._41,
					a._41*b._12 + a._42*b._22 + a._43*b._32 + a._44*b._42,
					a._41*b._13 + a._42*b._23 + a._43*b._33 + a._44*b._43,
					a._41*b._14 + a._42*b._24 + a._43*b._34 + a._44*b._44)
				);
		}

		SVector4 operator * (const SVector4& v)
		{
			return SVector4(
				m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
				m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
				m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
				m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
				);
		}

		/*
		operator D3DXMATRIX() const
		{
			D3DXMATRIX mtxOut;
			mtxOut._11 = _11; mtxOut._12 = _12; mtxOut._13 = _13; mtxOut._14 = _14;
			mtxOut._21 = _12; mtxOut._22 = _22; mtxOut._23 = _23; mtxOut._24 = _24;
			mtxOut._31 = _13; mtxOut._32 = _32; mtxOut._33 = _33; mtxOut._34 = _34;
			mtxOut._41 = _14; mtxOut._42 = _42; mtxOut._43 = _43; mtxOut._44 = _44;
			return mtxOut;
		}
		*/

	};

	typedef struct S_API SMatrix SMatrix4;

	static void SMatrixIdentity( SMatrix* pMtx )
	{
		if( pMtx != 0 )
		{
			pMtx->_11 = 1; pMtx->_12 = 0; pMtx->_13 = 0; pMtx->_14 = 0;
			pMtx->_21 = 0; pMtx->_22 = 1; pMtx->_23 = 0; pMtx->_24 = 0;
			pMtx->_31 = 0; pMtx->_32 = 0; pMtx->_33 = 1; pMtx->_34 = 0;
			pMtx->_41 = 0; pMtx->_42 = 0; pMtx->_43 = 0; pMtx->_44 = 1;
		}
	}
	
	static void SMatrixTranspose( SMatrix* pMtx )
	{
		if( pMtx != 0)
		{
			for( int n = 0; n < 4-2; n++ )
			{
				for( int m = n + 1; m < 4-1; m++ )
				{
					float fCache = pMtx->m[n][m];
					pMtx->m[n][m] = pMtx->m[m][n];
					pMtx->m[m][n] = fCache;
				}
			}
		}
	}		
}