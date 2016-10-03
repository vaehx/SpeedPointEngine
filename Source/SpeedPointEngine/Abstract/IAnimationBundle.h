// ********************************************************************************************

//	SpeedPoint Animation Bundle

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>


SP_NMSPACE_BEG

struct S_API IAnimationJoint;
struct S_API IAnimationSequence;
struct S_API ISolid;


// SpeedPoint Animation Bundle (abstract)
struct S_API IAnimationBundle
{
public:
	// Initialize the animation bundle with its parent solid
	virtual SResult Initialize( ISolid* pSolid ) = 0;

	// Add a new empty joint to this animation bundle
	virtual IAnimationJoint* AddJoint( SP_ID* pID ) = 0;

	// Get a joint by its id
	virtual IAnimationJoint* GetJoint( SP_ID id ) = 0;

	// Add an Animation Sequence to this bundle
	virtual IAnimationSequence* AddAnimationSequence( SP_ID* pID ) = 0;

	// Get an animation sequence by its id
	virtual IAnimationSequence* GetAnimationSequence( SP_ID id ) = 0;

	// Stop current animation seq. and run a specific Animation sequence
	// Restarts current animation sequence if it already runs
	virtual SResult StartAnimationSequence( SP_ID id ) = 0;

	// Get the currently running animation sequence
	virtual SP_ID GetRunningAnimationSequence( void ) = 0;

	// Tick the current animation sequence
	virtual SResult Tick( float fTimeFactor ) = 0;

	// Clearout the Bundle but including the animations that are stored in this bundle
	virtual void Clear( void ) = 0;
	
	//// TODO : Is that everything did we forgot any functions?
};


SP_NMSPACE_END