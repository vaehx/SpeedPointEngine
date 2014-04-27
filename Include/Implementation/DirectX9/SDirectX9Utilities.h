// ******************************************************************************************

//	SpeedPoint DirectX9 Utilities

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <d3dx9.h>
#include <Util\SMatrix.h>

namespace SpeedPoint
{	
	// Convert DirectX Matrix to SpeedPoint Matrix
	inline S_API SMatrix DXMatrixToSMatrix(const D3DXMATRIX& mat)
	{
		return SMatrix(
			SVector4(mat._11, mat._12, mat._13, mat._14),
			SVector4(mat._21, mat._22, mat._23, mat._24),
			SVector4(mat._31, mat._32, mat._33, mat._34),
			SVector4(mat._41, mat._42, mat._43, mat._44)
			);
	}

	// Convert SpeedPoint Matrix to DirectX Matrix
	inline S_API D3DXMATRIX SMatrixToDXMatrix(const SMatrix& mat)
	{
		return D3DXMATRIX(
			mat._11, mat._12, mat._13, mat._14,
			mat._21, mat._22, mat._23, mat._24,
			mat._31, mat._32, mat._33, mat._34,
			mat._41, mat._42, mat._43, mat._44
			);
	}

	// Convert D3DXMATRIX to D3DMATRIX
	inline S_API D3DMATRIX DXMatrixToDMatrix(const D3DXMATRIX& mat)
	{
		D3DMATRIX out;
		out._11 = mat._11; out._12 = mat._12; out._13 = mat._13; out._14 = mat._14;
		out._21 = mat._21; out._22 = mat._22; out._23 = mat._23; out._24 = mat._24;
		out._31 = mat._31; out._32 = mat._32; out._33 = mat._33; out._34 = mat._34;
		out._41 = mat._41; out._42 = mat._42; out._43 = mat._43; out._44 = mat._44;
		return out;
	}
}