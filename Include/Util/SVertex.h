// ********************************************************************************************

//	SpeedPoint Vertex Structure

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <Abstract\Vector2.h>
#include <Abstract\Vector3.h>

namespace SpeedPoint
{
	// Texture coordinates (UV) structure
	struct S_API TextureCoordsUV
	{
		float u, v;
		
		TextureCoordsUV()
			: u(0.0f), v(0.0f) {}
		TextureCoordsUV(const TextureCoordsUV& o)
			: u(o.u), v(o.v) {}
		TextureCoordsUV(const SVector2& vec)
			: u(vec.x), v(vec.y) {}
		TextureCoordsUV(float uu, float vv)
			: u(uu), v(vv) {}
	};

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
		TextureCoordsUV textureCoords[1];

		// ---------------------------------------------------------------------------------		
		
		// constructor with single components and custom normal and tangent, 2 texturecoord pairs
		SVertex (float x_,float y_,float z_, float nx_,float ny_,float nz_, float tx_,float ty_,float tz_, float tu_,float tv_, float tu2_,float tv2_)
			: x(x_), y(y_), z(z_),
			nx(nx_), ny(ny_), nz(nz_),
			tx(tx_), ty(ty_), tz(tz_)
		{				
			//binormal = Vec3f( nx, ny, nz ).Cross( Vec3f( tx, ty, ty ) );				
			textureCoords[0].u = tu_;
			textureCoords[0].v = tv_;
			//textureCoords[1].x = tu2_;
			//textureCoords[1].y = tv2_;
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

		// Calculate Tangent vector from stored normal and given up vector
		inline SVertex& CalcTangent(const SVertex& v2, const SVertex& v3)
		{
			float s1 = v2.textureCoords[0].u - textureCoords[0].u, t1 = v2.textureCoords[0].v - textureCoords[0].v;
			float s2 = v3.textureCoords[0].u - textureCoords[0].u, t2 = v3.textureCoords[0].v - textureCoords[0].v;
			Vec3f q1(v2.x - x, v2.y - y, v2.z - z), q2(v3.x - x, v3.y - y, v3.z - z);
			float r = 1.0f / (s1 * t2 - s2 * t1);
			float rt1neg = -r * t1, rt2 = r * t2;
			tx = rt2 * q1.x + rt1neg * q2.x;
			ty = rt2 * q1.y + rt1neg * q2.y;
			tz = rt2 * q1.z + rt1neg * q2.z;
			return *this;
		}

		// Get the binormal of this Vertex
		SXYZ GetBinormal()
		{
			return Vec3Cross(Vec3f(nx, ny, nz), Vec3f(tx, ty, ty));
		}
	};

	// normalize(cross(v2 - v1, v3 - v1))
	inline S_API Vec3f SPNormalFromThreeVerts(const SVertex& v1, const SVertex& v2, const SVertex& v3)
	{
		return Vec3Normalize(Vec3Cross(
			Vec3f(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z),
			Vec3f(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z)));
	}
}