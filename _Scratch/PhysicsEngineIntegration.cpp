
Physics Engine integration
Goal: Total encapsulation?
	-> NO! Instead use navigated dependency!

-----------------------------------------------------------------------------------------------

// Following example is allowed:

class SSpeedPointEngine
{
public:		
};

#include <SSolidSystem.h>
class SPhysicsEngine
{
public:
	SResult /* !!!! */ DoPhysics(SSolidSystem* /* !!! */ pSolidSystem);
};


-----------------------------------------------------------------------------------------------

// Following example is not allowed:

#include <SPhysicalSolid.h>
class SSpeedPointEngine
{
private:
	SPhysicsEngine m_pPhysicsEngine;	

public:
	SResult DoPhysics()
	{
		m_pPhysicsEngine->GetPhysicalSolid(...);
	}	
};

class SIPhysicsEngine
{
public:
	virtual ???? GetPhysicalSolid(...); // !! We'd need to forward declarate pointer types and this is just cruel crap
};

---

class SPhysicsEngine
{
public:
	SPhysicalSolid* GetPhysicalSolid(...);
};

int main()
{
	SSpeedPointEngine myEngine;
	myEngine.DoPhysics();
}


===============================================================================================

// best solution:
// 	integrate Physics engine, but do not force user to use it and do not depend on it
// 	This requires, that the physics engine hold its own Instances of the PhysicalSolid, which inherits the SBasicSolid class
// 	The PhysicsEngine can stay synched to the SpeedPoint Solids by using the event handling


// ****************************************************************************************************
// SSpeedPointEngine.h

#include <SFrameEngine.h>



// ****************************************************************************************************
// SPhysCollisionInfo.h

struct SPhysCollisionInfo
{
	bool		m_bCollided;		// Did it collide or not
	SVector3	m_CollisionTangent;	// Tangent between the colliding solids
	float		m_fForce;		// The approximated force of the collision
	float		m_fTimeDiff;		// part of fTimeDiff passed to CheckCollision function at which the collision took place
	// ....
};

// ****************************************************************************************************
// SPhysicalSolid.h
