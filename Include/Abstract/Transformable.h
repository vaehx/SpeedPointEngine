// ********************************************************************************************

//	SpeedPoint Transformable object

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix.h"
#include "BoundBox.h"


SP_NMSPACE_BEG

// SpeedPoint Transformable Object
class S_API STransformable
{
protected:	
	AABB m_AABB;
	SMatrix4	m_WorldMatrix;
	bool		m_WorldMatrixCalculated;	// whether the worldmtx ever has been calculated

public:
	Vec3f	vPosition;
	Quat	rotation;
	Vec3f	vSize;

	// Default constructor
	STransformable()
		: vSize(1.0f),
		m_WorldMatrixCalculated(false)		
	{		
	};

	// Constructor with position
	STransformable(const Vec3f& pos)
		: vPosition(pos),
		vSize(1.0f),
		m_WorldMatrixCalculated(false)
	{
	};

	// Constructor with position and rotation
	STransformable(const Vec3f& pos, const Quat& rot)
		: vPosition(pos),
		rotation(rot),
		vSize(1.0f),
		m_WorldMatrixCalculated(false)
	{
	};

	// Constructor with position, rotation and scale
	STransformable(const Vec3f& pos, const Quat& rot, const Vec3f& scale)
		: vPosition(pos),
		rotation(rot),
		vSize(scale),
		m_WorldMatrixCalculated(false)
	{
	};

	// Copy constructor
	STransformable(const STransformable& other)
		: vPosition(other.vPosition),
		rotation(other.rotation),
		vSize(other.vSize),
		m_WorldMatrixCalculated(false)
	{
	};

	// Destructor
	virtual ~STransformable()
	{
		m_WorldMatrixCalculated = false;
	}






	// Move absolute
	ILINE void Move(const Vec3f& pos)
	{
		m_AABB.MoveRelative(pos - vPosition);
		vPosition = pos;
		RecalculateWorldMatrix();
	}		

	// Move absolute
	ILINE void Move(float x, float y, float z)
	{
		Vec3f pos(x, y, z);
		m_AABB.MoveRelative(pos - vPosition);
		vPosition = pos;
		RecalculateWorldMatrix();
	}		

	// Move relative
	ILINE void Translate(const Vec3f& vec)
	{
		m_AABB.MoveRelative(vec);
		vPosition += vec;
		RecalculateWorldMatrix();
	}

	// Move relative
	ILINE void Translate(float x, float y, float z)
	{
		Vec3f v(x, y, z);
		m_AABB.MoveRelative(v);
		vPosition += v;
		RecalculateWorldMatrix();
	}
	
	// Rotate absolute	
	ILINE void SetRotation(const Quat& rot)
	{
		rotation = rot;
		RecalculateWorldMatrix();
		RecalcBoundBox();
	}

	// Rotate relative
	ILINE void Turn(const Vec3f& eulerAngles)
	{
		Quat turnQuat = Quat::FromEuler(eulerAngles);
		rotation = rotation * turnQuat;
		RecalculateWorldMatrix();
		RecalcBoundBox();
	}
	
	// Set size absolutely
	// Does not update BoundBox. Do this by overriding this function and RecalcBoundBox()
	ILINE void Size(const Vec3f& size)
	{
		vSize = size;
		RecalculateWorldMatrix();
		RecalcBoundBox();
	}		

	// Set size absolutely
	// Does not update BoundBox. Do this by overriding this function and RecalcBoundBox()
	ILINE void Size(float w, float h, float d)
	{
		vSize = Vec3f(w, h, d);
		RecalculateWorldMatrix();
		RecalcBoundBox();
	}		

	// Scale relatively
	// Does not update BoundBox. Do this by overriding this function and RecalcBoundBox()
	ILINE void Scale(const Vec3f& vec)
	{
		vSize += vec;
		RecalculateWorldMatrix();
		RecalcBoundBox();
	}
	
	// Scale relative
	// Does not update BoundBox. Do this by overriding this function and RecalcBoundBox()
	ILINE void Scale(float w, float h, float d)
	{
		vSize += Vec3f(w, h, d);
		RecalculateWorldMatrix();
		RecalcBoundBox();
	}		

	// Summary:
	//	Get a World Matrix from this transformable object
	ILINE SMatrix4& GetWorldMatrix()
	{
		if (!m_WorldMatrixCalculated)
			RecalculateWorldMatrix();

		return m_WorldMatrix;
	}

	// Summary:
	//	Recalculate the World matrix of this transformable object
	ILINE SMatrix4& RecalculateWorldMatrix()
	{
		// World / Form matrix
		//D3DXMATRIX mTrans, mRot, mScale, mOrig;
		//
		//D3DXMatrixIdentity(&mOrig);
		//D3DXMatrixRotationYawPitchRoll(&mRot, form->vRotation.y, form->vRotation.x, form->vRotation.z);
		//D3DXMatrixTranslation(&mTrans, form->vPosition.x, form->vPosition.y, form->vPosition.z);
		//D3DXMatrixScaling(&mScale, form->vSize.x, form->vSize.y, form->vSize.z);
		//
		//D3DXMATRIX mW = mOrig * mScale * mRot * mTrans;
		//
		//mWorld = DXMatrixToDMatrix(mW);

		SMatrix rotMtx = rotation.ToRotationMatrix();

		m_WorldMatrix = SMatrix4(	// identity
			SVector4(1.0f, 0, 0, 0),
			SVector4(0, 1.0f, 0, 0),
			SVector4(0, 0, 1.0f, 0),
			SVector4(0, 0, 0, 1.0f)
			) * SMatrix4(	// Size
			SVector4(vSize.x, 0, 0, 0),
			SVector4(0, vSize.y, 0, 0),
			SVector4(0, 0, vSize.z, 0),
			SVector4(0, 0, 0, 1.0f)
			) * rotMtx /** SMatrix4(		// Roll
			SVector4(cosf(vRotation.z), -sinf(vRotation.z), 0, 0),
			SVector4(sinf(vRotation.z), cosf(vRotation.z), 0, 0),
			SVector4(0, 0, 1.0f, 0),
			SVector4(0, 0, 0, 1.0f)
			) * SMatrix4(		// Pitch
			SVector4(1.0f, 0, 0, 0),
			SVector4(0, cosf(vRotation.x), -sinf(vRotation.x), 0),
			SVector4(0, sinf(vRotation.x), cosf(vRotation.x), 0),
			SVector4(0, 0, 0, 1.0f)
			) * SMatrix4(	// Yaw
			SVector4(cosf(vRotation.y), 0, sinf(vRotation.y), 0),
			SVector4(0, 1.0f, 0, 0),
			SVector4(-sinf(vRotation.y), 0, cosf(vRotation.y), 0),
			SVector4(0, 0, 0, 1.0f)
			)*/ * SMatrix4(	// Translation
			SVector4(1.0f, 0, 0, vPosition.x),
			SVector4(0, 1.0f, 0, vPosition.y),
			SVector4(0, 0, 1.0f, vPosition.z),
			SVector4(0, 0, 0, 1.0f)
			);

		m_WorldMatrixCalculated = true;

		return m_WorldMatrix;
	}				



	ILINE virtual void RecalcBoundBox()
	{
	}



	// Getter / Setter
public:
	Vec3f& GetPosition() { return vPosition; }

	Quat& GetRotation() { return rotation; }

	Vec3f& GetSize() { return vSize; }

	AABB& GetBoundBox() { return m_AABB; }
};


SP_NMSPACE_END