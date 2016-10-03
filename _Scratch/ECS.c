
enum EShaderType
{
	eSHADER_STANDARD,
	eSHADER_CLOTH,
	eSHADER_WATER,
	...
};


enum EIlluminationModel
{
	eILLUMMODEL_BLINN,
	eILLUMMODEL_COOKTORRANCE,
	...
};

struct StandardShaderParams
{
	float roughness;
	float metalness;
	float specularity;
	ITexture* textureMap;
	EIlluminationModel illum;
	...
};

struct IRenderer
{
	virtual void Render() = 0;
};

// ----
struct STransformable
{
	Vec3f position, rotation;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//			C O M P O N E N T S
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

enum EComponentType
{
	eCOMPONENT_RENDERABLE,
	eCOMPONENT_RIGIDBODY,
	eCOMPONENT_ANIMATEABLE,
	...
};

// Components are in IS-A relationship to Objects
struct IComponent
{
	virtual void Update(float fTimeDelta) = 0;
	virtual EComponentType GetType() const = 0;
};


class CRenderableComponent : public IComponent
{
private:
	IRenderer* m_pRenderer;
	Object* m_pObject;
	Geometry m_Geometry;
	SRenderScheduleSlot* m_pRSS;
public:
	CRenderableComponent(Object* pObject)
		: m_pObject(pObject)
	{
		m_pRenderer = m_pObject->GetEngine()->GetRenderer();
	}

	virtual void Update(float fTimeDelta)
	{
		// Create RSS if not there already
		if (!m_pRSS)
			m_pRSS = m_pRenderer->GetRenderScheduleSlot();

		// Update RSS
		m_pRSS->transform.position = m_pObject->position;
		m_pRSS->transform.rotation = m_pObject->rotation;
		m_pRSS->shader = eSHADER_STANDARD;
		m_pRSS->standardShaderParams.roughness = 0.3f;

		...
	}

	virtual EComponentType GetType() const { return eCOMPONENT_RENDERABLE; }
};

class CRigidBodyComponent : public IComponent
{
private:
	IGameEngine* m_pEngine;
	CollisionShape m_CollisionShape;
	...
public:
	CRigidBodyComponent(Object* pObject)
	{
		m_pEngine = pObject->GetEngine();
	}

	virtual void Update(float fTimeDelta)
	{
		std::vector<CollisionInfo> colls = DetectCollisions(m_CollisionShape);
		for (auto itCollision = colls.begin(); itCollision != colls.end(); itCollision++)
		{
			// Trigger sound
			ISoundSystem* pSoundSystem = m_pEngine->GetSoundSystem();
			pSoundSystem->TriggerSound(SoundLibrary::GetSoundByCollision(*itCollision));

			// No Decals added here, because is assumed to be done only for
			// bullets.
			
			// Apply Rigid-Body Damage
			CRigidBodyComponent* pRigidBody = (CRigidBodyComponent*)itCollision->GetOtherObject()->GetComponent(eCOMPONENT_RIGIDBODY);
			if (pRigidBody)		
				pRigidBody->AddDamage(itCollision->GetContactPoint(), itCollision->GetContactVelocity());
		}
	}

	virtual EComponentType GetType() const { return eCOMPONENT_RIGIDBODY; }
};

class CAnimateableComponent : public IComponent
{
private:
	float m_fPassedTime;	// ... since animation start
	std::vector<AnimationSequence> m_Sequences;
	AnimationSequence* m_pCurrentSequence;
	Geometry* m_pGeometry;
public:
	CAnimateableComponet(Geometry* pGeometry)
		: m_pGeometry(pGeometry)
	{
	}

	bool SetSequence(const char* sequence_name)
	{
		for (auto itSequence = m_Sequences.begin(); itSequence != m_Sequences.end(); itSequence++)
		{
			if (stricmp(itSequence->GetName(), sequence_name) == 0)
			{
				m_pCurrentSequence = &(*itSequence);
				m_fPassedTime = 0;
				return true;
			}
		}
		return false;
	}

	virtual void Update(float fTimeDelta)
	{
		if (!m_pCurrentSequence)
			return;

		m_pCurrentSequence->ApplyKeyFrame(m_pGeometry, m_fPassedTime);
		m_fPassedTime += fTimeDelta;
	}

	virtual EComponenType GetType() const { return eCOMPONENT_ANIMATEABLE; }
};




/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//			O B J E C T S
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

class Object : public STransformable
{
protected:
	IGameEngine* m_pEngine;
	std::vector<IComponent*> m_Components;
public:
	Object(IGameEngine* pEngine)
		: m_pEngine(pEngine)
	{
	}

	virtual ~Object()
	{
		Clear();
	}

	IGameEngine* GetEngine() { return m_pEngine; }

	IComponent* GetComponent(EComponenType type)
	{
		for (auto itComponent = m_Components.begin(); itComponent != m_Components.end(); itComponent++)
		{
			if (!(*itComponent))
				continue;

			if ((*itComponent)->GetType() == type)
				return *itComponent;		
		}
	}

	void AddComponent(IComponent* pComponent)
	{
		m_Components.push_back(pComponent);
	}

	virtual void Update(float fTimeDelta)
	{
		for (auto itComponent = m_Components.begin(); itComponent != m_Components.end(); itComponent++)
		{
			if (!(*itComponent))
				continue;

			(*itComponent)->Update(fTimeDelta);
		}
	}

	virtual void Clear()
	{
		for (auto itComponent = m_Components.begin(); itComponent != m_Components.end(); itComponent++)
		{
			if (!(*itComponent))
				continue;

			delete (*itComponent);
		}
		m_Components.clear();
	}
};



class Weapon : public Object
{
public:
	Weapon(IGameEngine* pEngine)
		: m_pEngine(pEngine)
	{
		AddComponent(new CRenderableComponent((Object*)this));
		AddComponent(new CRigidBodyComponent((Object*)this));
	}

	virtual void Clear()
	{
		Object::Clear();
		// Do own clear stuff here...
	}
};




























