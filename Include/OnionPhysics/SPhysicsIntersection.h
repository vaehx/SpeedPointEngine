// **************************************************************************************

// Onion Physics Intersection Testing Methods

// **************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <Util\SVector2.h>
#include <Util\SVector3.h>

namespace SpeedPoint
{
#define O_NONE 0

// Plane / Side of a cylinder:
// Imagine a cylinder with v = (0,1,0)
// Then you have a bottom and top side + plane
#define O_BOTTOM_PLANE 1
#define O_TOP_PLANE 2

// Sphere of a capsule:
// Imagine a capsule with v = (0,1,0)
// Then you have a bottom and top sphere
#define O_BOTTOM_SPHERE 1
#define O_TOP_SPHERE 2

#define O_BOTTOM_CIRCLE 1
#define O_TOP_CIRCLE 2

#define O_BOTTOM_SIDE 1
#define O_TOP_SIDE 2
#define O_LEFT_SIDE 3
#define O_RIGHT_SIDE 4

	// =======================================================================================

	// Define the basic collision meshes
	struct S_API OCircle;	
	struct S_API OBox2;	// We use Box2 here instead of Rectangle as it is much shorter
	struct S_API OCapsule2;
	struct S_API OLine2;

	typedef struct S_API SVector2 OPoint2;

	struct S_API OSphere;
	struct S_API OBox;	
	struct S_API OCapsule;
	struct S_API OLine;
	struct S_API OPlane;
	struct S_API OCylinder;

	typedef struct S_API SVector3 OPoint;

	// ***********************************************************************************************************
	// ***********************************************************************************************************

	// Actual Calculation Functions that are called
	// NOTE: pvDistance is always the actual (minimum) remaining distance between the objects

	// 2d

	class S_API SPhysics2DIntersect
	{
	public:
		static bool CircleCircle( OCircle* pCirc1, OCircle* pCirc2, float2* pvDistance );
		static bool CircleBox2( OCircle* pCirc, OBox2* pBox, float2* pvDistance, short* pnSide );
		static bool CircleCapsule2( OCircle* pCirc, OCapsule2* pCaps, float2* pvDistance, short* pnCircle );
		static bool CircleLine2( OCircle* pCirc, OLine2* pLine, float2* pvDistance, float2* pvIntPnt1, float2* pvIntPnt2 );
		static bool CirclePoint2( OCircle* pCirc, OPoint2* pPoint, float2* pvDistance );

		static bool Box2Box2( OBox2* pBox1, OBox2* pBox2, float2* pvDistance, short* pnSide1, short* pnSide2 );
		static bool Box2Capsule2( OBox2* pBox, OCapsule2* pCaps, float2* pvDistance, short* pnSide, short* pnCircle );
		static bool Box2Line2( OBox2* pBox, OLine2* pLine, float2* pvDistance, short* pnSide, float2* pvIntPnt1, float2* pvIntPnt2 );
		static bool Box2Point2( OBox2* pBox, OPoint2* pPoint, float2* pvDistance );
		static bool Box2Circle( OBox2* pBox, OCircle* pCirc, float2* pvDistance, short* pnSide )
			{ return CircleBox2( pCirc, pBox, pvDistance, pnSide ); }

		static bool Capsule2Capsule2( OCapsule2* pCaps1, OCapsule2* pCaps2, float2* pvDistance, short* pnCircle1, short* pnCircle2 );
		static bool Capsule2Line2( OCapsule2* pCaps, OLine2* pLine, float2* pvDistance, bool* pbParallel, short* pnCircle );
		static bool Capsule2Point2( OCapsule2* pCaps, OPoint2* pPoint, float2* pvDistance, short* pnCircle );
		static bool Capsule2Circle( OCapsule2* pCaps, OCircle* pCirc, float2* pvDistance, short* pnCircle )
			{ return CircleCapsule2( pCirc, pCaps, pvDistance, pnCircle ); }
		static bool Capsule2Box2( OCapsule2* pCaps, OBox2* pBox, float2* pvDistance, short* pnCircle, short* pnSide )
			{ return Box2Capsule2( pBox, pCaps, pvDistance, pnSide, pnCircle ); }

		static bool Line2Line2( OLine2* pLine1, OLine* pLine2, float2* pvDistance, float* pS1, float* pS2, bool* pbParallel );
		static bool Line2Point2( OLine2* pLine, OPoint2* pPoint, float2* pvDistance );
		static bool Line2Circle( OLine2* pLine, OCircle* pCircle, float2* pvDistance, float2* pvIntPnt1, float2* pvIntPnt2 )
			{ return CircleLine2( pCircle, pLine, pvDistance, pvIntPnt1, pvIntPnt2 ); }
		static bool Line2Box2( OLine2* pLine, OBox2* pBox, float2* pvDistance, float2* pvIntPnt1, float2* pvIntPnt2, short* pnSide )
			{ return Box2Line2( pBox, pLine, pvDistance, pnSide, pvIntPnt1, pvIntPnt2 ); }
		static bool Line2Capsule2( OLine2* pLine, OCapsule2* pCaps, float2* pvDistance, bool* pbParallel, short* pnCircle )
			{ return Capsule2Line2( pCaps, pLine, pvDistance, pbParallel, pnCircle ); }

		static bool Point2Point2( OPoint2* pPoint1, OPoint2* pPoint2, float2* pvDistance );
		static bool Point2Circle( OPoint2* pPoint, OCircle* pCirc, float2* pvDistance )
			{ return CirclePoint2( pCirc, pPoint, pvDistance ); }
		static bool Point2Box2( OPoint2* pPoint, OBox2* pBox, float2* pvDistance )
			{ return Box2Point2( pBox, pPoint, pvDistance ); }
		static bool Point2Capsule2( OPoint2* pPoint, OCapsule2* pCaps, float2* pvDistance, short* pnCircle )
			{ return Capsule2Point2( pCaps, pPoint, pvDistance, pnCircle ); }
		static bool Point2Line2( OPoint2* pPoint, OLine2* pLine, float2* pvDistance )
			{ return Line2Point2( pLine, pPoint, pvDistance ); }
	};


	// 3d
	
	class S_API SPhysics3DIntersect
	{
	public:
		static bool SphereSphere( OSphere* pSphere1, OSphere* pSphere2, float3* pvDistance, OPoint* pvCollisionPoint, float* pfIntCircleRad );
		static bool SphereBox( OSphere* pSphere, OBox* pBox, float3* pvDistance, short* nPlanesColliding, float3* pvNormalAverage );
		static bool SphereCapsule( OSphere* pSphere, OCapsule* pCapsule, float3* pvDistance, short* pnSphere );
		static bool SphereLine( OSphere* pSphere, OLine* pLine, float3* pvDistance, float* pS, float3* pvIntPnt1, float3* pvIntPnt2 );
		static bool SpherePlane( OSphere* pSphere, OPlane* pPlane, float3* pvDistance, float3* pvIntCircleCenter, float* pfIntCircleRad );
		static bool SphereCylinder( OSphere* pSphere, OCylinder* pCyl, float3* pvDistance, short* piPlane );
		static bool SpherePoint( OSphere* pSphere, OPoint* pPoint, float3* pvDistance );

		static bool BoxBox( OBox* pBox1, OBox* pBox2, float3* pvDistance, short* nPlanesColliding, float3* pvNormalAverage );	
		static bool BoxCapsule( OBox* pBox, OCapsule* pCapsule, float3* pvDistance, short* pnPlanesColliding, float3* pvNormalAverage, short* pnSphere );
		static bool BoxLine( OBox* pBox, OLine* pLine, float3* pvDistance, short* pnPlanesColliding, float3* pvNormalAverage, float3* pvIntPnt1, float3* pvIntPnt2 );
		static bool BoxPlane( OBox* pBox, OPlane* pPlane, float3* pvDistance, short* pnPlanesColliding, float3* pvNormalAverage );
		static bool BoxCylinder( OBox* pBox, OCylinder* pCyl, float3* pvDistance, short* pnPlanesColliding, float3* pvNormalAverage, short* pnCylPlane );
		static bool BoxPoint( OBox* pSphere, OPoint* pPoint, float3* pvDistance );
		static bool BoxSphere( OBox* pBox, OSphere* pSphere, float3* pvDistance, short* pnPlanesColliding, float3* pvNormalAverage )
			{ return SphereBox( pSphere, pBox, pvDistance, pnPlanesColliding, pvNormalAverage ); }

		static bool CapsuleCapsule( OCapsule* pCapsule1, OCapsule* pCapsule2, float3* pvDistance, short* pnSphere1, short* pnSphere2 );
		static bool CapsuleLine( OCapsule* pCaps, OLine* pLine, float3* pvDistance, short* pnSphere, bool* pbParallel );
		static bool CapsulePlane( OCapsule* pCaps, OPlane* pPlane, float3* pvDistance, bool* pbParallel, short* pnSphere );
		static bool CapsuleCylinder( OCapsule* pCaps, OCylinder* pCyl, float3* pvDistance, bool* pbParallel, short* pnSphere, short* pnCylPlane );
		static bool CapsulePoint( OCapsule* pCaps, OPoint* pPoint, float3* pvDistance );
		static bool CapsuleSphere( OCapsule* pCaps, OSphere* pSphere, float3* pvDistance, short* pnSphere )
			{ return SphereCapsule( pSphere, pCaps, pvDistance, pnSphere ); }
		static bool CapsuleBox( OCapsule* pCaps, OBox* pBox, float3* pvDistance, short* pnSphere, short* pnPlanesColliding, float3* pvNormalAverage )
			{ return BoxCapsule( pBox, pCaps, pvDistance, pnPlanesColliding, pvNormalAverage, pnSphere ); }

		static bool LineLine( OLine* pLine1, OLine* pLine2, float3* pvDistance, bool* pbParallel, float3* pvIntPnt, float* pS1, float* pS2 );
		static bool LinePlane( OLine* pLine, OPlane* pPlane, float3* pvIntPnt, float* pS, float3* pvDistance, bool* pbParallel );
		static bool LineCylinder( OLine* pLine, OCylinder* pCyl, float3* pvDistance, short* pnPlane, bool* pbParallel );
		static bool LinePoint( OLine* pLine, OPoint* pPoint, float3* pvDistance );
		static bool LineSphere( OLine* pLine, OSphere* pSphere, float3* pvDistance, float* pS, float3* pvIntPnt1, float3* pvIntPnt2 )
			{ return SphereLine( pSphere, pLine, pvDistance, pS, pvIntPnt1, pvIntPnt2 ); }
		static bool LineBox( OLine* pLine, OBox* pBox, float3* pvDistance, float3* pvIntPnt1, float3* pvIntPnt2, short* pnPlanesColliding, float3* pvNormalAverage )
			{ return BoxLine( pBox, pLine, pvDistance, pnPlanesColliding, pvNormalAverage, pvIntPnt1, pvIntPnt2 ); }
		static bool LineCapsule( OLine* pLine, OCapsule* pCaps, float3* pvDistance, short* pnSphere, bool* pbParallel )
			{ return CapsuleLine( pCaps, pLine, pvDistance, pnSphere, pbParallel ); }	

		static bool PlanePlane( OPlane* pPlane1, OPlane* pPlane2, float3* pvDistance, OLine* pIntLine, bool* pbParallel );
		static bool PlaneCylinder( OPlane* pPlane, OCylinder* pCyl, float3* pvDistance, short* pnSphere );
		static bool PlanePoint( OPlane* pPlane, OPoint* pPoint, float3* pvDistance );
		static bool PlaneSphere( OPlane* pPlane, OSphere* pSphere, float3* pvDistance, float3* pvIntCircleCenter, float* pfIntCircleRad )
			{ return SpherePlane( pSphere, pPlane, pvDistance, pvIntCircleCenter, pfIntCircleRad ); }
		static bool PlaneBox( OPlane* pPlane, OBox* pBox, float3* pvDistance, short* pnPlanesColliding, float3* pvNormalAverage )
			{ return BoxPlane( pBox, pPlane, pvDistance, pnPlanesColliding, pvNormalAverage ); }
		static bool PlaneCapsule( OPlane* pPlane, OCapsule* pCaps, float3* pvDistance, bool* pbParallel, short* pnSphere )
			{ return CapsulePlane( pCaps, pPlane, pvDistance, pbParallel, pnSphere ); }
		static bool PlaneLine( OPlane* pPlane, OLine* pLine, float3* pvIntPnt, float3* pvDistance, float* pS, bool* pbParallel )
			{ return LinePlane( pLine, pPlane, pvIntPnt, pS, pvDistance, pbParallel ); }	

		static bool CylinderCylinder( OCylinder* pCyl1, OCylinder* pCyl2, float3* pvDistance, short* piPlane1, short* piPlane2 );
		static bool CylinderPoint( OCylinder* pCyl, OPoint* pPoint, float3* pvDistance );
		static bool CylinderSphere( OCylinder* pCyl, OSphere* pSphere, float3* pvDistance, short* piPlane )
			{ return SphereCylinder( pSphere, pCyl, pvDistance, piPlane ); }
		static bool CylinderBox( OCylinder* pCyl, OBox* pBox, float3* pvDistance, short* pnCylPlane, short* pnPlanesColliding, float3* pvNormalAverage )
			{ return BoxCylinder( pBox, pCyl, pvDistance, pnPlanesColliding, pvNormalAverage, pnCylPlane ); }
		static bool CylinderCapsule( OCylinder* pCyl, OCapsule* pCaps, float3* pvDistance, bool* pbParallel, short* pnCylPlane, short* pnSphere )
			{ return CapsuleCylinder( pCaps, pCyl, pvDistance, pbParallel, pnSphere, pnCylPlane ); }
		static bool CylinderLine( OCylinder* pCyl, OLine* pLine, float3* pvDistance, short* pnPlane, bool* pbParallel )
			{ return LineCylinder( pLine, pCyl, pvDistance, pnPlane, pbParallel ); }
		static bool CylinderPlane( OCylinder* pCyl, OPlane* pPlane, float3* pvDistance, short* pnSphere )
			{ return PlaneCylinder( pPlane, pCyl, pvDistance, pnSphere ); }

		static bool PointPoint( OPoint* pPoint1, OPoint* pPoint2, float3* pvDistance );
		static bool PointSphere( OPoint* pPoint, OSphere* pSphere, float3* pvDistance )
			{ return SpherePoint( pSphere, pPoint, pvDistance ); }
		static bool PointBox( OPoint* pPoint, OBox* pBox, float3* pvDistance )
			{ return BoxPoint( pBox, pPoint, pvDistance ); }
		static bool PointCapsule( OPoint* pPoint, OCapsule* pCapsule, float3* pvDistance )
			{ return CapsulePoint( pCapsule, pPoint, pvDistance ); }
		static bool PointLine( OPoint* pPoint, OLine* pLine, float3* pvDistance )
			{ return LinePoint( pLine, pPoint, pvDistance ); }
		static bool PointPlane( OPoint* pPoint, OPlane* pPlane, float3* pvDistance )
			{ return PlanePoint( pPlane, pPoint, pvDistance ); }
		static bool PointCylinder( OPoint* pPoint, OCylinder* pCyl, float3* pvDistance )
			{ return CylinderPoint( pCyl, pPoint, pvDistance ); }	
	};

	// ***********************************************************************************************************


	// ***********************************************************************************************************
	// A 2d Line
	struct S_API OLine2
	{
	public:
		OPoint2 p;
		float2	v;

		OLine2() {}
		OLine2( const OPoint2& pSupport, const float2& vDirection ) : p( pSupport ), v( vDirection ) {}

		OPoint2 GetPoint( float s ) { return p + s * v; }		
	};

	// ***********************************************************************************************************
	// A 2d Circle
	struct S_API OCircle
	{
	public:
		OPoint2	p;	// position
		float	rad;	// radius

		OCircle() : rad( 0.0f ) {};
		OCircle( const OPoint2& pCenter, const float& fRadius ) : p( pCenter ), rad( fRadius ) {};
	};

	// ***********************************************************************************************************
	// A 2d Box (Rectangle)
	struct S_API OBox2
	{
	public:
		OPoint2	p;	// Center point
		float2	size;	// Sizes (in all directions)
		float	alpha;	// rotation of this rectangle		

		OBox2() : alpha( 0.0f ) {}
		OBox2( const OPoint2& pCenter, const float2& vSize ) : p( pCenter ), size( vSize ), alpha( 0 ) {}
		OBox2( const OPoint2& pCenter, const float2& vSize, const float& fAlpha )
			: p( pCenter ), size( vSize ), alpha( fAlpha ) {};
	};

	// ***********************************************************************************************************
	// A 2d Capsule
	struct S_API OCapsule2
	{
	public:
		OPoint2 p;	// Center point
		float2	v;	// Direction
		float	rad;	// radius for both: inner cylinder and spheres
		float	length;	// Length from one sphere center to the other

		OCapsule2() : rad( 0 ), length( 0 ) {}
		OCapsule2( const OPoint2& pCenter, const float2& vDirection, const float& fRadius, const float& fLength )
			: p( pCenter ), rad( fRadius ), length( fLength ), v( vDirection ) {}
	};

	// ***********************************************************************************************************
	// Sphere Collision Shape
	struct S_API OSphere
	{
	public:
		OPoint	p;	// Center point
		float	rad;	// Radius

		OSphere() : rad( 0 ) {}
		OSphere( const OPoint& pCenter, const float& fRadius ) : p( pCenter ), rad( fRadius ) {}
	};

	// ***********************************************************************************************************
	// Box Collision Shape
	struct S_API OBox
	{
	public:
		OPoint	p;	// Center point
		float3	size;	// Size
		float3	rot;	// Rotation (roll, yaw, pitch) as radians

		OBox() {};
		OBox( const OPoint& pCenter ) : p( pCenter ) {};
		OBox( const OPoint& pCenter, const float3& vSize, const float3& vRot ) : p( pCenter ), size( vSize ), rot( vRot ) {};
	};

	// ***********************************************************************************************************
	// Capsule Collision Shape
	struct S_API OCapsule
	{
	public:
		OPoint	p;	// Center point
		float3	v;	// Direction of the capsule
		float	rad;	// Radius for both: inner cylinder and spheres
		float	length; // Length from on sphere center to the other

		OCapsule() : rad( 0 ), length( 0 ) {};
		OCapsule( const OPoint& pCenter, const float3& vDirection, const float& fRadius, const float& fLength )
			: p( pCenter ), v( vDirection ), rad( fRadius ), length( fLength ) {};
	};

	// ***********************************************************************************************************
	// Line Collision Shape
	struct S_API OLine
	{
	public:
		OPoint	p;	// Support vector
		float3	v;	// Direction vector

		OLine() {};
		OLine( const OPoint& pSupport, const float3& vDirection ) : p( pSupport ), v( vDirection ) {};

		OPoint GetPoint( float s ) { return p + s * v; }
		OPoint GetPerpendicularFoot( OPoint* pPoint );
	};	

	// ***********************************************************************************************************
	// Plane Collision Shape (cartesian form)
	struct S_API OPlane
	{
	public:
		float3	n;	// Plane normal
		float	d;	// Distance to origin

		OPlane() : d( 0 ) {};
		OPlane( const float3& vNormal, const float& fDistance ) : n( vNormal ), d( fDistance ) {};			

		OPoint GetPerpendicularFoot( OPoint* pPoint );
	};

	// ***********************************************************************************************************
	// Cylinder Collision Shape
	struct S_API OCylinder
	{
	public:
		OPoint	p;	// Support vector of middle line
		float3	v;	// Direction vector of middle line
		float	length;	// Length of the cylinder

		OCylinder() : length( 0 ) {};
		OCylinder( const OPoint& pCenter, const float3& vDirection, const float& fLength )
			: p( pCenter ), v( vDirection ), length( fLength ) {};
	};
}