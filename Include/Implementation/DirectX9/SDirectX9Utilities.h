// ******************************************************************************************

//	SpeedPoint DirectX9 Utilities

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <d3dx9.h>
#include <SMatrix.h>

namespace SpeedPoint
{
	// Convert DirectX Matrix to SpeedPoint Matrix
	inline SMatrix DXMatrixToSMatrix(const D3DXMATRIX& mat)
	{
		return SMatrix(
			SVector4(mat._11, mat._12, mat._13, mat._14),
			SVector4(mat._21, mat._22, mat._23, mat._24),
			SVector4(mat._31, mat._32, mat._33, mat._34),
			SVector4(mat._41, mat._42, mat._43, mat._44)
			);
	}

	// Convert SpeedPoint Matrix to DirectX Matrix
	inline D3DXMATRIX SMatrixToDXMatrix(const SMatrix& mat)
	{
		return D3DXMATRIX(
			mat._11, mat._12, mat._13, mat._14,
			mat._21, mat._22, mat._23, mat._24,
			mat._31, mat._32, mat._33, mat._34,
			mat._41, mat._42, mat._43, mat._44
			);
	}
}