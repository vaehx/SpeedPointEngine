// ********************************************************************************************

//	SpeedPoint Vertex Structure

// ********************************************************************************************

#include "SPrerequisites.h"
#include "SVector2.h"

namespace SpeedPoint
{

	// SpeedPoint Vertex structure
	struct S_API SVertex 
	{	
		float			x, y, z;
		float			nx, ny, nz;	
		SVector2		avTexture[2];

		// --

		SVertex () {};

		SVertex (float x_, float y_, float z_, float nx_, float ny_, float nz_, float tu_, float tv_)
			: x(x_), y(y_), z(z_), nx(nx_), ny(ny_), nz(nz_) { avTexture[0].x = tu_; avTexture[0].y = tv_; };

		SVertex (float x_, float y_, float z_, float nx_, float ny_, float nz_, float tu_, float tv_, float tu2_, float tv2_)
			: x(x_), y(y_), z(z_), nx(nx_), ny(ny_), nz(nz_) { avTexture[0].x = tu_; avTexture[0].y = tv_; avTexture[1].x = tu2_; avTexture[1].y = tv2_; };
	};

}