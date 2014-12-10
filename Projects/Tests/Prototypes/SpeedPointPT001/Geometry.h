

#pragma once

#include <SpeedPointGeometry.h>

namespace GeometryLib
{
	
	// Summary:
	//	Generate a box with given sizes, each side made of 2 triangles. 24 Vertices, 36 Indices
	static SpeedPoint::SResult GenerateBox(SpeedPoint::IGameEngine* pEngine, SpeedPoint::IGeometry* pGeometry, float w, float h, float d)
	{
		if (!IS_VALID_PTR(pGeometry))
			return SpeedPoint::S_INVALIDPARAM;

		pGeometry->Clear();

		SpeedPoint::SInitialGeometryDesc initialGeom;
		initialGeom.pVertices = new SpeedPoint::SVertex[24];
		initialGeom.pIndices = new SpeedPoint::SIndex[36];

		float wh = w * 0.5f, hh = h * 0.5f, dh = d * 0.5f;

		//						  Position		   Normal		Tangent		    UV
		// Top:
		initialGeom.pVertices[ 0] = SpeedPoint::SVertex( wh, hh,-dh,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		initialGeom.pVertices[ 1] = SpeedPoint::SVertex(-wh, hh,-dh,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		initialGeom.pVertices[ 2] = SpeedPoint::SVertex(-wh, hh, dh,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		initialGeom.pVertices[ 3] = SpeedPoint::SVertex (wh, hh, dh,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

		// Back:
		initialGeom.pVertices[ 4] = SpeedPoint::SVertex( wh, hh, dh,	0.0f, 0.0f, 1.0f,	0, 0, 0, 0.0f, 0.0f);
		initialGeom.pVertices[ 5] = SpeedPoint::SVertex(-wh, hh, dh,	0.0f, 0.0f, 1.0f,	0, 0, 0, 1.0f, 0.0f);
		initialGeom.pVertices[ 6] = SpeedPoint::SVertex(-wh,-hh, dh,	0.0f, 0.0f, 1.0f,	0, 0, 0, 1.0f, 1.0f);
		initialGeom.pVertices[ 7] = SpeedPoint::SVertex( wh,-hh, dh,	0.0f, 0.0f, 1.0f,	0, 0, 0, 0.0f, 1.0f);

		// Bottom:
		initialGeom.pVertices[ 8] = SpeedPoint::SVertex( wh,-hh, dh,	0.0f, -1.0f, 0.0f,	0, 0, 0, 0.0f, 0.0f);
		initialGeom.pVertices[ 9] = SpeedPoint::SVertex(-wh,-hh, dh,	0.0f, -1.0f, 0.0f,	0, 0, 0, 1.0f, 0.0f);
		initialGeom.pVertices[10] = SpeedPoint::SVertex(-wh,-hh,-dh,	0.0f, -1.0f, 0.0f,	0, 0, 0, 1.0f, 1.0f);
		initialGeom.pVertices[11] = SpeedPoint::SVertex( wh,-hh,-dh,	0.0f, -1.0f, 0.0f,	0, 0, 0, 0.0f, 1.0f);

		// Front:
		initialGeom.pVertices[12] = SpeedPoint::SVertex( wh,-hh,-dh,	0.0f, 0.0f, -1.0f,	0, 0, 0, 0.0f, 0.0f);
		initialGeom.pVertices[13] = SpeedPoint::SVertex(-wh,-hh,-dh,	0.0f, 0.0f, -1.0f,	0, 0, 0, 1.0f, 0.0f);
		initialGeom.pVertices[14] = SpeedPoint::SVertex(-wh, hh,-dh,	0.0f, 0.0f, -1.0f,	0, 0, 0, 1.0f, 1.0f);
		initialGeom.pVertices[15] = SpeedPoint::SVertex( wh, hh,-dh,	0.0f, 0.0f, -1.0f,	0, 0, 0, 0.0f, 1.0f);

		// Left:
		initialGeom.pVertices[16] = SpeedPoint::SVertex( wh,-hh, dh,	1.0f, 0.0f, 0.0f,	0, 0, 0, 0.0f, 0.0f);
		initialGeom.pVertices[17] = SpeedPoint::SVertex( wh,-hh,-dh,	1.0f, 0.0f, 0.0f,	0, 0, 0, 1.0f, 0.0f);
		initialGeom.pVertices[18] = SpeedPoint::SVertex( wh, hh,-dh,	1.0f, 0.0f, 0.0f,	0, 0, 0, 1.0f, 1.0f);
		initialGeom.pVertices[19] = SpeedPoint::SVertex( wh, hh, dh,	1.0f, 0.0f, 0.0f,	0, 0, 0, 0.0f, 1.0f);

		// Right:
		initialGeom.pVertices[20] = SpeedPoint::SVertex(-wh,-hh,-dh,	-1.0f, 0.0f, 0.0f,	0, 0, 0, 0.0f, 0.0f);
		initialGeom.pVertices[21] = SpeedPoint::SVertex(-wh,-hh, dh,	-1.0f, 0.0f, 0.0f,	0, 0, 0, 1.0f, 0.0f);
		initialGeom.pVertices[22] = SpeedPoint::SVertex(-wh, hh, dh,	-1.0f, 0.0f, 0.0f,	0, 0, 0, 1.0f, 1.0f);
		initialGeom.pVertices[23] = SpeedPoint::SVertex(-wh, hh,-dh,	-1.0f, 0.0f, 0.0f,	0, 0, 0, 0.0f, 1.0f);


		initialGeom.nVertices = 24;

		// Top:
		initialGeom.pIndices[0] = 0; initialGeom.pIndices[1] = 1; initialGeom.pIndices[2] = 2;
		initialGeom.pIndices[3] = 2; initialGeom.pIndices[4] = 3; initialGeom.pIndices[5] = 0;

		// Back:
		initialGeom.pIndices[6] = 4; initialGeom.pIndices[7] = 5; initialGeom.pIndices[8] = 6;
		initialGeom.pIndices[9] = 6; initialGeom.pIndices[10] = 7; initialGeom.pIndices[11] = 4;

		// Bottom:
		initialGeom.pIndices[12] = 8; initialGeom.pIndices[13] = 9; initialGeom.pIndices[14] = 10;
		initialGeom.pIndices[15] = 10; initialGeom.pIndices[16] = 11; initialGeom.pIndices[17] = 8;

		// Front:
		initialGeom.pIndices[18] = 12; initialGeom.pIndices[19] = 13; initialGeom.pIndices[20] = 14;
		initialGeom.pIndices[21] = 14; initialGeom.pIndices[22] = 15; initialGeom.pIndices[23] = 12;

		// Left:
		initialGeom.pIndices[24] = 16; initialGeom.pIndices[25] = 17; initialGeom.pIndices[26] = 18;
		initialGeom.pIndices[27] = 18; initialGeom.pIndices[28] = 19; initialGeom.pIndices[29] = 16;

		initialGeom.pIndices[30] = 20; initialGeom.pIndices[31] = 21; initialGeom.pIndices[32] = 22;
		initialGeom.pIndices[33] = 22; initialGeom.pIndices[34] = 23; initialGeom.pIndices[35] = 20;
		initialGeom.nIndices = 36;		

		// calculate tangents	
		for (unsigned int i = 0; i < initialGeom.nIndices; i += 3)
		{
			unsigned int i1 = initialGeom.pIndices[i], i2 = initialGeom.pIndices[i + 1], i3 = initialGeom.pIndices[i + 2];
			SpeedPoint::SVertex &v1 = initialGeom.pVertices[i1], &v2 = initialGeom.pVertices[i2], &v3 = initialGeom.pVertices[i3];
			v1.CalcTangent(v2, v3);
			v2.tx = v1.tx; v2.ty = v1.ty; v2.tz = v1.tz;
			v3.tx = v1.tx; v3.ty = v1.ty; v3.tz = v1.tz;
		}

		SpeedPoint::SResult res = pGeometry->Init(pEngine, pEngine->GetRenderer(), &initialGeom);
		delete[] initialGeom.pIndices; initialGeom.pIndices = nullptr;
		delete[] initialGeom.pVertices; initialGeom.pVertices = nullptr;

		return res;
	}





	static void GenerateSphere(SpeedPoint::IGameEngine* pEngine, SpeedPoint::IGeometry* pGeometry, float radius, const SpeedPoint::SVector3& offset)
	{

	}
}