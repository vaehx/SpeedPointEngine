// SpeedPoint: Onion Physics 3D Intersection testing methods

#include <OnionPhysics\SPhysicsIntersection.h>

namespace SpeedPoint
{

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::SphereSphere( OSphere* pSphere1, OSphere* pSphere2, float3* pvDistance, OPoint* ppCollision, float* pfIntCircleRad )
	{
		if( pSphere1 == NULL || pSphere2 == NULL ) return false;
		
		float3 delta = pSphere2->p - pSphere1->p;
		float length = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;		
		float radSum = pSphere1->rad + pSphere2->rad;
		if( length <= radSum * radSum )
		{
			float dist = 0;
			if( pvDistance )
			{
				length = sqrtf( length ); // Maybe we can somehow avoid this squareroot?
				dist = length - radSum;
				*pvDistance = ( delta / length ) * dist;
			}

			// Calculate tangent point of vector between two sphere centers
			if( ppCollision )
			{
				if( !pvDistance )
				{
					length = sqrtf( length );

					// This is propably not needed here
					//dist = length - radSum;
				}

				if( dist == 0 )
					*ppCollision = pSphere1->p + ( delta / length ) * pSphere1->rad;
			}

			// Calculate the radius of the Intersection circle
			if( pfIntCircleRad )
			{
				/////// TODO
			}

			return true;
		}

		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::SphereBox( OSphere* pSphere, OBox* pBox, float3* pvDistance, short* nPlanesColliding, float3* pvNormalAverage )
	{
		if( pSphere == NULL || pBox == NULL ) return false;
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::SphereCapsule( OSphere* pSphere, OCapsule* pCapsule, float3* pvDistance, short* pnSphere )
	{
		if( pSphere == NULL || pCapsule == NULL ) return false;
		
		float3 v = SpeedPoint::SVector3Normalize( pCapsule->v );		
		float d = v.x * pSphere->p.x + v.y * pSphere->p.y + v.z * pSphere->p.z;		
		float s = ( d - v.x * pCapsule->p.x - v.y * pCapsule->p.y - v.z * pCapsule->p.z )
			/ ( v.x * v.x + v.y * v.y + v.z * v.z + 0.00001f );		
		float3 F = pCapsule->p + s * v;	
		float fFToCenter = abs( s );

		float3 delta = F - pSphere->p;
		float length = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;		
		float radSum = pSphere->rad + pCapsule->rad;
		float dist = 0;
				
		if( pnSphere ) *pnSphere = O_NONE;
				
		// now get nearest sphere
		// remember v is the normalized direction of the capsule
		float3* pNearestSphereCenter;	// Center of the sphere of the capsule, nearest to the sphere		
		float3* pProperDelta;		// Delta of Sphere center to pNearestSphereCenter
		float3 csph1 = pCapsule->p + ( pCapsule->length * 0.5f ) * v;
		float3 csph2 = pCapsule->p - ( pCapsule->length * 0.5f ) * v;		
		float3 delta1 = pSphere->p - csph1;
		float3 delta2 = pSphere->p - csph2;

		// Note: We avoid squareroot here
		if( delta1.x * delta1.x + delta1.y * delta1.y + delta1.z * delta1.z <
			delta2.x * delta2.x + delta2.y * delta2.y + delta2.z * delta2.z )
		{
			pNearestSphereCenter = &csph1;
			pProperDelta = &delta1;			
		}
		else
		{
			pNearestSphereCenter = &csph2;
			pProperDelta = &delta2;
		}		

		// Check if sphere collides with infinite cylinder at all		
		bool bCollidesAtAll = ( length <= radSum * radSum );

		float3 vDistance;

		// The length between sphere center and the nearest sphere of the capsule
		float deltaLength = SVector3Length( *pProperDelta );

		// Now get the distance
		if( pvDistance || bCollidesAtAll )
		{								
			// we need to check distance of F to center of Capsule before we go			
			if( fFToCenter < pCapsule->length * 0.5f )
			{
				// inside the inner cylinder
				length = sqrtf( length ); // maybe we can avoid this squareroot?
				dist = length - pSphere->rad - pCapsule->rad;				
				if( pvDistance )
					*pvDistance = ( delta / length ) * dist;
				else
					vDistance = ( delta / length ) * dist;
			}				
			else
			{
				// F inside a sphere or outside the capsule				
				if( pvDistance )
					*pvDistance = ( *pProperDelta / deltaLength ) * ( deltaLength - radSum );
				else
					vDistance = ( *pProperDelta / deltaLength ) * ( deltaLength - radSum );
			}
		}

		if( bCollidesAtAll )
		{
			if( fFToCenter < pCapsule->length * 0.5f )
			{
				// Inside the inner cylinder. so definetly collides
				return true;
			}
			else if( deltaLength - radSum <= 0 )
			{
				// Sphere collides with nearest sphere of the capsule
				if( pnSphere )
				{
					// check which sphere
					if( pNearestSphereCenter == &csph1 )
						*pnSphere = O_TOP_SPHERE;
					else
						*pnSphere = O_BOTTOM_SPHERE;
				}

				return true;
			}
		}

		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::SphereLine( OSphere* pSphere, OLine* pLine, float3* pvDistance, float* pS, float3* pvIntPnt1, float3* pvIntPnt2 )
	{
		if( pSphere == NULL || pLine == NULL ) return false;

		// first, get distance of line with point + F
		float3 v = SpeedPoint::SVector3Normalize( pLine->v );		
		float d = v.x * pSphere->p.x + v.y * pSphere->p.y + v.z * pSphere->p.z;		
		float s = ( d - v.x * pLine->p.x - v.y * pLine->p.y - v.z * pLine->p.z )
			/ ( v.x * v.x + v.y * v.y + v.z * v.z + 0.00001f );		
		if( pS ) *pS = s;
		float3 F = pLine->p + s * v;
		float3 delta = pSphere->p - F;
		float dist = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;		
		
		// Check distance with radius now
		if( dist > pSphere->rad * pSphere->rad )
		{
			if( pvDistance )
			{
				dist = sqrtf( dist );			
				*pvDistance = v * dist;
			}

			return false;
		}

		// Line does intersect with sphere
		// Now calculate intersection points

		///// BEGIN TODO

		if( pvIntPnt1 )
		{
			*pvIntPnt1 = F;
			if( pvIntPnt2 )
			{
				*pvIntPnt2 = F;
			}
		}

		///// END TODO

		return true;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::SpherePlane( OSphere* pSphere, OPlane* pPlane, float3* pvDistance, float3* pvIntCircleCenter, float* pfIntCircleRad )
	{
		if( pSphere == NULL || pPlane == NULL ) return false;

		// First get distance to center point
		float3 vDistance;
		PlanePoint( pPlane, &pSphere->p, &vDistance );
		float fLength = SpeedPoint::SVector3Length( vDistance );
		float fDist = fLength - pSphere->rad;
		bool bCollides = ( fDist <= 0 );
		
		// Center of the intersection circle
		if( pvIntCircleCenter )
		{
			if( !bCollides )
				*pvIntCircleCenter = pPlane->GetPerpendicularFoot( &pSphere->p );
			else
				*pvIntCircleCenter = pSphere->p;
		}

		// Calculate the radius of the intersection circle
		if( pfIntCircleRad )
		{
			if( !bCollides )
			{
				// Does not collide at all, so no radius
				*pfIntCircleRad = 0;
			}
			else
			{
				float theta = ( 1.570796327f * fLength ) / pSphere->rad;
				*pfIntCircleRad = pSphere->rad * cosf( theta );
			}
		}

		// Calculate the distance vector
		if( pvDistance )			
			*pvDistance = vDistance - vDistance * ( pSphere->rad / fLength );	

		return bCollides;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::SphereCylinder( OSphere* pSphere, OCylinder* pCyl, float3* pvDistance, short* piPlane )
	{
		if( pSphere == NULL || pCyl == NULL ) return false;

		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::SpherePoint( OSphere* pSphere, OPoint* pPoint, float3* pvDistance )
	{
		if( pSphere == NULL || pPoint == NULL ) return false;
		float3 delta = pSphere->p - *pPoint;
		float dist = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;

		// Calculate the distance vector
		if( pvDistance )
		{
			float length = sqrtf( dist );
			*pvDistance = *pPoint + ( delta / length ) * ( length - pSphere->rad );
		}

		return ( dist <= pSphere->rad * pSphere->rad );
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::BoxBox( OBox* pBox1, OBox* pBox2, float3* pvDistance, short* nPlanesColliding, float3* pvNormalAverage )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::BoxCapsule( OBox* pBox, OCapsule* pCapsule, float3* pvDistance, short* pnPlanesColliding, float3* pvNormalAverage, short* pnSphere )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::BoxLine( OBox* pBox, OLine* pLine, float3* pvDistance, short* pnPlanesColliding, float3* pvNormalAverage, float3* pvIntPnt1, float3* pvIntPnt2 )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::BoxPlane( OBox* pBox, OPlane* pPlane, float3* pvDistance, short* pnPlanesColliding, float3* pvNormalAverage )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::BoxCylinder( OBox* pBox, OCylinder* pCyl, float3* pvDistance, short* pnPlanesColliding, float3* pvNormalAverage, short* pnCylPlane )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::BoxPoint( OBox* pSphere, OPoint* pPoint, float3* pvDistance )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::CapsuleCapsule( OCapsule* pCapsule1, OCapsule* pCapsule2, float3* pvDistance, short* pnSphere1, short* pnSphere2 )
	{
		if( pCapsule1 == NULL || pCapsule2 == NULL ) return false;
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::CapsuleLine( OCapsule* pCaps, OLine* pLine, float3* pvDistance, short* pnSphere, bool* pbParallel )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::CapsulePlane( OCapsule* pCaps, OPlane* pPlane, float3* pvDistance, bool* pbParallel, short* pnSphere )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::CapsuleCylinder( OCapsule* pCaps, OCylinder* pCyl, float3* pvDistance, bool* pbParallel, short* pnSphere, short* pnCylPlane )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::CapsulePoint( OCapsule* pCaps, OPoint* pPoint, float3* pvDistance )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::LineLine( OLine* pLine1, OLine* pLine2, float3* pvDistance, bool* pbParallel, float3* pvIntPnt, float* pS1, float* pS2 )
	{
		if( pLine1 == NULL || pLine2 == NULL ) return false;		
		
		// Check if the lines are parallel		
		bool bParallel = ( pLine1->v == pLine2->v || pLine1->v == -pLine2->v );
		if( pbParallel ) *pbParallel = bParallel;		

		bool bCollides = false;
		if( bParallel )
		{
			// Try point intersection on that line
			if( LinePoint( pLine1, &pLine2->p, NULL ) )
			{
				// -> the lines are equal
				bCollides = true;
			}
		}

		// Calculate the distance vector		
		if( pvDistance )
		{
			float3 vDist = SpeedPoint::SVector3Cross( pLine1->v, pLine2->v );
			*pvDistance = vDist;			
		}

		// Check if they intersect and where		
		if( !bParallel )
		{
			// g1: x1 = p1 + s1 * v1
			// g2: x2 = p2 + s2 * v2
			// -> p1 + s1 * v1 = p2 + s2 * v2
			// dp = p2 - p1
			// so:
			float3 dp = pLine2->p - pLine1->p;
			float3 v1 = pLine1->v;
			float3 v2 = pLine2->v;
			float denominator = ( v1.x * v2.x * v2.y - v1.y );
			if( abs( denominator ) > 0.0001f )
			{
				float s2, s1;
				
				s2 = ( ( ( dp.x * v1.y ) / v2.x ) - v1.x * v2.x * pLine2->p.y ) / denominator;
				if( pS2 ) *pS2 = s2;				
					
				// so we can calculate s1 now
				s1 = ( dp.x + s2 * v2.x ) / v1.x;
				if( pS1 ) *pS1 = s1;				

				if( pLine1->GetPoint( s1 ) == pLine2->GetPoint( s2 ) ) bCollides = true;

				// Calculate the intersection point				
				if( pvIntPnt )
				{
					if( bCollides )
						*pvIntPnt = pLine1->GetPoint( s1 );
				}
			}
		}

		// Calculate the intersection point
		if( bCollides )
		{
			if( bParallel ) *pvIntPnt = pLine1->p;
		}
		else
			*pvIntPnt = float3();

		return bCollides;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::LinePlane( OLine* pLine, OPlane* pPlane, float3* pvIntPnt, float* pS, float3* pvDistance, bool* pbParallel )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::LineCylinder( OLine* pLine, OCylinder* pCyl, float3* pvDistance, short* pnPlane, bool* pbParallel )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::LinePoint( OLine* pLine, OPoint* pPoint, float3* pvDistance )
	{
		if( pLine == NULL || pPoint == NULL ) return false;		

		// first, check if the point lies on the line
		// g: q = p + s * v
		// -> s = ( q - p ) / v
		bool bPointLiesOnLine = false;
		float s1 = ( pPoint->x - pLine->p.x ) / pLine->v.x;
		float s2 = ( pPoint->y - pLine->p.y ) / pLine->v.y;
		if( s1 == s2 )
		{
			float s3 = ( pPoint->z - pLine->p.z ) / pLine->v.z;
			if( s3 == s2 ) bPointLiesOnLine = true;
		}

		if( pvDistance )
		{
			if( bPointLiesOnLine ) *pvDistance = float3();
			else *pvDistance = *pPoint - pLine->GetPerpendicularFoot( pPoint );
		}

		return bPointLiesOnLine;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::PlanePlane( OPlane* pPlane1, OPlane* pPlane2, float3* pvDistance, OLine* pIntLine, bool* pbParallel )
	{
		if( pPlane1 == NULL || pPlane2 == NULL ) return false;

		// check if they are parallel
		bool bParallel = ( pPlane1->n == pPlane2->n || pPlane1->n == -pPlane2->n );
		if( pbParallel ) *pbParallel = bParallel;

		bool bCollides = !bParallel
			|| ( pPlane1->n == pPlane2->n && pPlane1->d == pPlane2->d )
			|| ( pPlane1->n == -pPlane2->n && pPlane1->d == -pPlane2->d );

		// Calculate distance vector
		if( pvDistance )
		{
			if( bParallel )
			{				
				// first make the normals equal
				float3 n2 = pPlane2->n;
				float d2 = pPlane2->d;				
				if( pPlane1->n == -n2 )
				{
					n2 = -n2;
					d2 = -d2;
				}

				// Get any point in plane 2 with (x,0,0)
				// -> E: a*x + b*0 + c*0 = d -> x = d / a
				float x = d2 / n2.x;

				// Then calculate distance from this point
				// d = abs( ( a*x + b*y + c*z - d1 ) / length( n1 ) )
				// -> d = abs( ( a*x - d1 ) / length( n1 ) )
				float n1length = SpeedPoint::SVector3Length( pPlane1->n );
				float dist = abs( ( pPlane1->n.x * x - pPlane1->d ) / n1length );

				// Now get the proper direction vector
				*pvDistance = dist * ( pPlane1->n / n1length );
			}
			else
				*pvDistance = float3();
		}

		// Calculate intersection line
		if( pIntLine && !bParallel )
		{
			float d1 = pPlane1->d, d2 = pPlane2->d;
			float a1 = pPlane1->n.x;
			float b1 = pPlane1->n.y, b2 = pPlane2->n.y;
			float c1 = pPlane1->n.z, c2 = pPlane2->n.z;			
			float denom = c2 - a1 * c1;
			pIntLine->p.x = ( ( d1 * c2 - d2 * c1 ) / a1 - 2 * d1 * c1 ) / denom;
			pIntLine->p.y = 0;
			pIntLine->p.z = ( d2 - d1 * d1 ) / denom;
			pIntLine->v.x = -( b2 / a1 - b1 ) / denom - b1 / a1;
			pIntLine->v.y = 1;
			pIntLine->v.z = -( b2 - b1 * a1 ) / denom;
			pIntLine->v = SpeedPoint::SVector3Normalize( pIntLine->v );
		}

		return bCollides;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::PlaneCylinder( OPlane* pPlane, OCylinder* pCyl, float3* pvDistance, short* pnSphere )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::PlanePoint( OPlane* pPlane, OPoint* pPoint, float3* pvDistance )
	{
		if( pPlane == NULL || pPoint == NULL ) return false;

		// E: ax + by + cz = d
		float formula1 = pPlane->n.x * pPoint->x + pPlane->n.y * pPoint->y + pPlane->n.z * pPoint->z;
		
		// Check if point lies in the plane
		if( formula1 == pPlane->d )
		{
			if( pvDistance ) *pvDistance = float3( 0 );
			return true;
		}

		// Not - so calculate the distance if wanted
		if( pvDistance )
		{
			// to speed up, we check if the normal is axis aligned					
			float fDist = 0;			
			if( pPlane->n.x == 0 && pPlane->n.z == 0 )
				fDist = ( pPlane->n.y * pPoint->y - pPlane->d ) / pPlane->n.y;
			else if( pPlane->n.y == 0 && pPlane->n.z == 0 )
				fDist = ( pPlane->n.x * pPoint->x - pPlane->d ) / pPlane->n.x;
			else if( pPlane->n.x == 0 && pPlane->n.y == 0 )
				fDist = ( pPlane->n.z * pPoint->z - pPlane->d ) / pPlane->n.z;
			else
				fDist = ( formula1 - pPlane->d ) / SpeedPoint::SVector3Length( pPlane->n );

			*pvDistance = pPlane->n * fDist;
		}

		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::CylinderCylinder( OCylinder* pCyl1, OCylinder* pCyl2, float3* pvDistance, short* piPlane1, short* piPlane2 )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::CylinderPoint( OCylinder* pCyl, OPoint* pPoint, float3* pvDistance )
	{
		return false;
	}

	// **********************************************************************************

	S_API bool SPhysics3DIntersect::PointPoint( OPoint* pPoint1, OPoint* pPoint2, float3* pvDistance )
	{
		if( pPoint1 == NULL || pPoint2 == NULL ) return false;
		if( pPoint1->x == pPoint2->x && pPoint1->y == pPoint2->y && pPoint1->z == pPoint1->z )
		{
			if( pvDistance )
				*pvDistance = *pPoint2 - *pPoint1;

			return true;
		}
		
		return false;
	}





	// **********************************************************************************
	// **********************************************************************************

	//	UTILITY FUNCTIONS FOR SEVERAL 3D SHAPES



	S_API OPoint OLine::GetPerpendicularFoot( OPoint* pPoint )
	{
		return OPoint();
	}

	// **********************************************************************************

	S_API OPoint OPlane::GetPerpendicularFoot( OPoint* pPoint )
	{
		return OPoint();
	}
}