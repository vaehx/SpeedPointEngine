// ********************************************************************************************

//	This file is part of the SpeedPoint Game Engine!

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\Vector3.h>
#include <Util\SColor.h>

namespace SpeedPoint
{
	// SpeedPoint Light
	class S_API SLight
	{		
	public:
		// Light Shape related variable members
		S_LIGHT_TYPE	tyLightType;
		SVector3	vPosition;	// Center position of the light shape
		SVector3	vDirection;	// Direction for directional light or Spot light
		float		fRadius;	// Radius of point light or footer radius of cone
		float		fLength;	// Length of the cone-light

		// Lighting Calculation related variable members
		float		fShineRadius;	// Radius for the shine effect
		SColor		colColor;	// Color of the light

		// Default constructor
		SLight()
			: tyLightType(S_LIGHT_POINT),
			fRadius(1.0f),
			fLength(2.0f),
			fShineRadius(0.2f),
			colColor(0.8f, 0.8f, 0.8f)
		{
		}

		// Copy constructor
		SLight(const SLight& o)
			: tyLightType(o.tyLightType),
			vPosition(o.vPosition),
			vDirection(o.vDirection),
			fRadius(o.fRadius),
			fLength(o.fLength),
			fShineRadius(o.fShineRadius),
			colColor(o.colColor)
		{
		}		

		// Check if this light is in a specified range
		bool IsInRange(float fRange, const SVector3& pos);
	};

}