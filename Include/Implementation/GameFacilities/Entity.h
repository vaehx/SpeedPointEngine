#pragma once

#include <Abstract\IEntity.h>
#include <vector>

using std::vector;

SP_NMSPACE_BEG


///////////////////////////////////////////////////////////////////////////////////////
//
//			Entity
//
///////////////////////////////////////////////////////////////////////////////////////

class S_API CEntity : public IEntity
{
private:
	vector<IComponent*> m_Components;

	// List of components that will be destructed by the entity
	// instead of a subsystem.
	vector<IComponent*> m_ManagedComponents;

	string m_Name;

	Vec3f m_Pos, m_Scale, m_Pivot;
	Quat m_Rot;

	bool m_bTransformInvalid;
	SMatrix m_Transform;

	void OnEntityTransformed();

protected:
	ILINE virtual void AddComponentIntrl(IComponent* pComponent, bool managed = false);

public:
	CEntity();

	ILINE virtual void Clear();

	ILINE virtual const Vec3f& GetPos() const;
	ILINE virtual void SetPos(const Vec3f& pos);
	ILINE virtual void Translate(const Vec3f& translate);

	ILINE virtual const Quat& GetRotation() const;
	ILINE virtual void SetRotation(const Quat& rotation);
	ILINE virtual void Rotate(const Quat& rotate);

	ILINE virtual const Vec3f& GetScale() const;
	ILINE virtual void SetScale(const Vec3f& scale);

	ILINE virtual const Vec3f& GetPivot() const;
	ILINE virtual void SetPivot(const Vec3f& pivot);

	ILINE virtual const char* GetName() const;
	ILINE virtual void SetName(const char* name);

	ILINE virtual const SMatrix& GetTransform();

	ILINE virtual AABB GetAABB();
	ILINE virtual AABB GetWorldAABB();

	ILINE virtual bool HasComponent(IComponent* pComponent) const;
	ILINE virtual void ReleaseComponent(IComponent* pComponent);
	ILINE virtual unsigned int GetNumComponents() const;
	ILINE virtual IComponent* GetComponentByIndex(unsigned int index) const;
};




SP_NMSPACE_END
