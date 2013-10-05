// ********************************************************************************************

//	(OnionPhysics) SpeedPoint Physical Object Material

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>

namespace SpeedPoint
{
#define S_DEFAULT_DENSITY 0.5f
#define S_DEFAULT_FRICTION 0.2f
#define S_DEFAULT_LIQUIDITY 0.0f
#define S_DEFAULT_DESTRUCTIONLIMIT 100.0f

	// Presets for physics effects
	enum S_API S_PHYSICSMATERIAL_PRESET
	{
		S_PHYSICSMATERIAL_GLASS,
		S_PHYSICSMATERIAL_METAL,
		S_PHYSICSMATERIAL_WOOD,
		S_PHYSICSMATERIAL_PLANT,
		S_PHYSICSMATERIAL_STONE,
		S_PHYSICSMATERIAL_LIVING,
		S_PHYSICSMATERIAL_WATER,
		S_PHYSICSMATERIAL_OIL
	};

	// (OnionPhysics) SpeedPoint Material for a physical object
	struct S_API SPhysicsMaterial
	{	
		float				fDensity;
		float				fFriction;
		float				fLiquidity;		// only for SPhysicsLiquid
		float				fDestructionLimit;

		bool				bCustom;
		S_PHYSICSMATERIAL_PRESET	prtPreset;

		// Standart Constructor
		SPhysicsMaterial()
			: fDensity( S_DEFAULT_DENSITY ),
			fFriction( S_DEFAULT_FRICTION ),
			fLiquidity( S_DEFAULT_LIQUIDITY ),
			fDestructionLimit( S_DEFAULT_DESTRUCTIONLIMIT ),
			bCustom( true ),
			prtPreset( S_PHYSICSMATERIAL_STONE ) {}

		// Paremeter Constructor
		SPhysicsMaterial( S_PHYSICSMATERIAL_PRESET preset )
			: fDensity( S_DEFAULT_DENSITY ),
			fFriction( S_DEFAULT_FRICTION ),
			fLiquidity( S_DEFAULT_LIQUIDITY ),
			fDestructionLimit( S_DEFAULT_DESTRUCTIONLIMIT ),
			bCustom( false ),
			prtPreset( preset ) {}

		// Copy constructor
		SPhysicsMaterial( const SPhysicsMaterial& other )
			: fDensity( other.fDensity ),
			fFriction( other.fFriction ),
			fLiquidity( other.fLiquidity ),
			fDestructionLimit( other.fDestructionLimit ),
			bCustom( other.bCustom ),
			prtPreset( other.prtPreset ) {}
	};
}