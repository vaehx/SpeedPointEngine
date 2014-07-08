// ********************************************************************************************

//	SpeedPoint Vertex Structure

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include "SVector2.h"
#include "SVector3.h"

namespace SpeedPoint
{
	// SpeedPoint Vertex structure
	struct S_API SVertex 
	{	
		// position
		union
		{
			struct
			{
				float x, y, z;				
			};
			SXYZ position;						
		};
		
		// normal
		union
		{			
			struct
			{
				float nx, ny, nz;
			};
			SXYZ normal;
		};
		
		// tangent
		union	
		{
			struct
			{
				float tx, ty, tz;
			};		
			SXYZ tangent;		
		};											

		// texture coords							
		SVector2 textureCoords[2];

		// ---------------------------------------------------------------------------------		
		
		// constructor with single components and custom normal and tangent, 2 texturecoord pairs
		SVertex (float x_,float y_,float z_, float nx_,float ny_,float nz_, float tx_,float ty_,float tz_, float tu_,float tv_, float tu2_,float tv2_)
			: x(x_), y(y_), z(z_),
			nx(nx_), ny(ny_), nz(nz_),
			tx(tx_), ty(ty_), tz(tz_)
		{				
			//binormal = SVector3( nx, ny, nz ).Cross( SVector3( tx, ty, ty ) );				
			textureCoords[0].x = tu_;
			textureCoords[0].y = tv_;
			textureCoords[1].x = tu2_;
			textureCoords[1].y = tv2_;
		}
		
		// constructor with single components and custom normal, 1 texturecoord pair
		SVertex (float x_,float y_,float z_, float nx_,float ny_,float nz_, float tx_,float ty_,float tz_, float tu_,float tv_)
			: SVertex (x_,y_,z_, nx_,ny_,nz_, tx_,ty_,tz_, tu_,tv_, 0.0f,0.0f)			
		{
		}
		
		// constructor with single components and custom normal, without texture coords
		SVertex (float x_,float y_,float z_, float nx_,float ny_,float nz_, float tx_,float ty_,float tz_)
			: SVertex (x_,y_,z_, nx_,ny_,nz_, tx_,ty_,tz_, 0.0f,0.0f, 0.0f,0.0f)
		{		
		}			
		
		// constructor with single components with normal pointing into y-direction
		SVertex (float x_,float y_,float z_)
			: SVertex (x_,y_,z_, 0.0f,1.0f,0.0f, 1.0f,0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f)
		{
		}								
		
		// Vertex at origin with normal pointing into y-direction
		SVertex ()
			: SVertex (0.0f,0.0f,0.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f)
		{			
		}

		// Get the binormal of this Vertex
		SXYZ GetBinormal()
		{
			return SVector3Cross(SVector3(nx, ny, nz), SVector3(tx, ty, ty));
		}
	};

}