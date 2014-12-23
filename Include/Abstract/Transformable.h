// ********************************************************************************************

//	SpeedPoint Transformable object

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include "Vector3.h"
#include "Matrix.h"


SP_NMSPACE_BEG

// SpeedPoint Transformable Object
class S_API STransformable
{
private:
	SMatrix4	m_WorldMatrix;
	bool		m_WorldMatrixCalculated;	// whether the worldmtx ever has been calculated

public:
	SVector3	vPosition;
	SVector3	vRotation;
	SVector3	vSize;

	// Default constructor
	STransformable()
		: vSize(1.0f),
		m_WorldMatrixCalculated(false)
	{
	};

	// Constructor with position
	STransformable(const SVector3& pos)
		: vPosition(pos),
		vSize(1.0f),
		m_WorldMatrixCalculated(false)
	{
	};

	// Constructor with position and rotation
	STransformable(const SVector3& pos, const SVector3& rot)
		: vPosition(pos),
		vRotation(rot),
		vSize(1.0f),
		m_WorldMatrixCalculated(false)
	{
	};

	// Constructor with position, rotation and scale
	STransformable(const SVector3& pos, const SVector3& rot, const SVector3& scale)
		: vPosition(pos),
		vRotation(rot),
		vSize(scale),
		m_WorldMatrixCalculated(false)
	{
	};

	// Copy constructor
	STransformable(const STransformable& other)
		: vPosition(other.vPosition),
		vRotation(other.vRotation),
		vSize(other.vSize),
		m_WorldMatrixCalculated(false)
	{
	};

	// Destructor
	~STransformable()
	{
		m_WorldMatrixCalculated = false;
	}






	ILINE void Move(const SVector3& pos)
	{
		vPosition = pos;
		RecalculateWorldMatrix();
	}		

	ILINE void Move(float x, float y, float z)
	{
		vPosition = SVector3(x, y, z);
		RecalculateWorldMatrix();
	}		

	ILINE void Translate(const SVector3& vec)
	{
		vPosition += vec;
		RecalculateWorldMatrix();
	}

	ILINE void Translate(float x, float y, float z)
	{
		vPosition += SVector3(x, y, z);
		RecalculateWorldMatrix();
	}
	
	ILINE void Rotate(const SVector3& rotation)
	{
		vRotation = rotation;
		RecalculateWorldMatrix();
	}		

	ILINE void Rotate(float p, float y, float r)
	{
		vRotation = SVector3(p, y, r);
		RecalculateWorldMatrix();
	}

	ILINE void Turn(const SVector3& vec)
	{
		vRotation += vec;
		RecalculateWorldMatrix();
	}

	ILINE void Turn(float p, float y, float r)
	{
		vRotation += SVector3(p, y, r);
		RecalculateWorldMatrix();
	}
	
	ILINE void Size(const SVector3& size)
	{
		vSize = size;
		RecalculateWorldMatrix();
	}		

	ILINE void Size(float w, float h, float d)
	{
		vSize = SVector3(w, h, d);
		RecalculateWorldMatrix();
	}		

	ILINE void Scale(const SVector3& vec)
	{
		vSize += vec;
		RecalculateWorldMatrix();
	}
	
	ILINE void Scale(float w, float h, float d)
	{
		vSize += SVector3(w, h, d);
		RecalculateWorldMatrix();
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
			) * SMatrix4(	// Yaw
			SVector4(cosf(vRotation.y), 0, sinf(vRotation.y), 0),
			SVector4(0, 1.0f, 0, 0),
			SVector4(-sinf(vRotation.y), 0, cosf(vRotation.y), 0),
			SVector4(0, 0, 0, 1.0f)
			) * SMatrix4(		// Pitch
			SVector4(1.0f, 0, 0, 0),
			SVector4(0, cosf(vRotation.x), -sinf(vRotation.x), 0),
			SVector4(0, sinf(vRotation.x), cosf(vRotation.x), 0),
			SVector4(0, 0, 0, 1.0f)
			) * SMatrix4(		// Roll
			SVector4(cosf(vRotation.z), -sinf(vRotation.z), 0, 0),
			SVector4(sinf(vRotation.z), cosf(vRotation.z), 0, 0),
			SVector4(0, 0, 1.0f, 0),
			SVector4(0, 0, 0, 1.0f)
			) * SMatrix4(	// Translation
			SVector4(1.0f, 0, 0, vPosition.x),
			SVector4(0, 1.0f, 0, vPosition.y),
			SVector4(0, 0, 1.0f, vPosition.z),
			SVector4(0, 0, 0, 1.0f)
			);

		m_WorldMatrixCalculated = true;

		return m_WorldMatrix;
	}					



	// Getter / Setter
public:
	SVector3& GetPosition() { return vPosition; }

	SVector3& GetRotation() { return vRotation; }

	SVector3& GetSize() { return vSize; }
};


SP_NMSPACE_END