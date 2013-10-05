// SpeedPoint: Onion Physics 2D

#include <OnionPhysics\SPhysics.h>

namespace SpeedPoint
{

	// **********************************************************************************

	S_API bool SPhysics2D::CircleCircle( OCircle* pCirc1, OCircle* pCirc2, float2* pvDistance )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics2D::CircleBox2( OCircle* pCirc, OBox2* pBox, float2* pvDistance, short* pnSide )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics2D::CircleCapsule2( OCircle* pCirc, OCapsule2* pCaps, float2* pvDistance, short* pnCircle )
	{
		return false;
	}

	// **********************************************************************************
	
	S_API bool SPhysics2D::CircleLine2( OCircle* pCirc, OLine2* pLine, float2* pvDistance, float2* pvIntPnt1, float2* pvIntPnt2 )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics2D::CirclePoint2( OCircle* pCirc, OPoint2* pPoint, float2* pvDistance )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics2D::Box2Box2( OBox2* pBox1, OBox2* pBox2, float2* pvDistance, short* pnSide1, short* pnSide2 )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics2D::Box2Capsule2( OBox2* pBox, OCapsule2* pCaps, float2* pvDistance, short* pnSide, short* pnCircle )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics2D::Box2Line2( OBox2* pBox, OLine2* pLine, float2* pvDistance, short* pnSide, float2* pvIntPnt1, float2* pvIntPnt2 )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics2D::Box2Point2( OBox2* pBox, OPoint2* pPoint, float2* pvDistance )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics2D::Capsule2Capsule2( OCapsule2* pCaps1, OCapsule2* pCaps2, float2* pvDistance, short* pnCircle1, short* pnCircle2 )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics2D::Capsule2Line2( OCapsule2* pCaps, OLine2* pLine, float2* pvDistance, bool* pbParallel, short* pnCircle )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics2D::Capsule2Point2( OCapsule2* pCaps, OPoint2* pPoint, float2* pvDistance, short* pnCircle )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics2D::Line2Line2( OLine2* pLine1, OLine* pLine2, float2* pvDistance, float* pS1, float* pS2, bool* pbParallel )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics2D::Line2Point2( OLine2* pLine, OPoint2* pPoint, float2* pvDistance )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics2D::Point2Point2( OPoint2* pPoint1, OPoint2* pPoint2, float2* pvDistance )
	{
		return false;
	}
}