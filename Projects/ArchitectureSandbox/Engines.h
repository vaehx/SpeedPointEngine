
// Engines





#pragma once


enum EResult
{
	S_ERROR,
	S_SUCCESS
};


struct IFrameEngine;
struct IAIEngine;
struct IPhysicsEngine;
struct IAnimationEngine;
struct IScriptEngine;
struct I3DEngine;
struct IRenderer;

struct IEngineModule
{
	virtual EResult Initialize(I3DEngine* gameEngine) = 0;
	virtual EResult Clear() = 0;
};



////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////









//	C O M M O N 









////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

// Note:
//	A stage has nothing to do with a pass!
enum ERenderingPipelineStage
{
	eRPS_GEOMETRY,
	eRPS_LIGHTING,	// only reached by deferred pipelines
	eRPS_COMPOSE,	// only reached by deferred lighting pipeline	
	eRPS_POST
};

struct IClientListener
{
	virtual EResult OnEngineInitialized() = 0;
	virtual EResult OnPreRenderingPipelineStage(ERenderingPipelineStage stage) = 0;
};
typedef struct IClientListener IClient;

// Usage:
//	inherit this class and overwrite the events you want
class ApplicationClient : IClient
{
public:
	ApplicationClient();
	~ApplicationClient();

	virtual EResult OnEngineInitialized();
	virtual EResult OnPreRenderingPipelineStage(ERenderingPipelineStage stage)
	{
		if (stage == eRPS_GEOMETRY)
			return OnRenderGeometry();

		else
			return S_SUCCESS;
	}

	EResult OnRenderGeometry();
};

// Example inherition of ApplicationClient class
class MyApplicationClient : public ApplicationClient
{
	EResult OnRenderGeometry()
	{
		// do something here.
		return S_SUCCESS;
	}
};




struct IFrameEngine
{
	virtual EResult Initialize() = 0;
	virtual EResult Clear() = 0;

	virtual EResult BeginFrame() = 0;
	virtual EResult EndFrame() = 0;
};


struct IAIEngine : IEngineModule
{
};


struct IPhysicalWorld;

struct IPhysicsEngine : IEngineModule
{
	virtual EResult CreatePhysicalWorld(IPhysicalWorld** pWorld) = 0;	
};

struct IAnimationEngine : IEngineModule
{
};

struct IScriptEngine : IEngineModule
{
};

struct I3DEngine : IEngineModule
{
};

struct IRenderer : IEngineModule
{
	virtual EResult BeginScene() = 0;
	virtual EResult EndScene() = 0;
	
	virtual EResult Flush() = 0;
};


// Entities:

// Description:
//	struct to provide a fixed-length char array and their operations
struct SName
{
	char* pChars;

	SName();
	SName(SName& o);
	SName(const char* name);

	bool operator == (const SName& n);
	bool operator != (const SName& n);
	SName& operator = (const SName& n);
	SName operator + (const SName& n);
};
#define SNameReference SName&
#define SNameRef SNameReference

struct INamed
{
	virtual void SetName(const SName& name) = 0;
	virtual SName& GetName() = 0;
};


class Named : public virtual INamed
{
private:
	SName m_Name;

public:
	virtual void SetName(const SName& name)
	{
		m_Name = name;
	}

	virtual SName& GetName()
	{
		return m_Name;
	}
};



struct IScript;

struct IEntityClass : public virtual INamed	// a base interface for every script extension class
{		
	virtual void SetScript(IScript* pScriptInstance) = 0;
	virtual IScript* GetScript() = 0;
	virtual EResult ExecuteScript() = 0;
};

struct IEntity
{
	virtual EResult SetID(unsigned int id) = 0;
	virtual unsigned int GetID() = 0;

	virtual EResult SetName(const char* name) = 0;
	virtual char* GetName() const = 0;
	
	// Arguments:
	//	pEntityClassInstance - pass a ptr to the instance of a class implementing IEntityClass interface
	virtual EResult AddEntityClass(IEntityClass* pEntityClassInstance) = 0;
};

// This is a sample IEntityClass Implementation






// Physics:


struct IPhysical
{
};

// physical script class
struct PhysicalSC : IEntityClass, Named
{

};

struct IPhysicalWorld
{
	virtual EResult AddPhysicalEntity(IEntity* pEntity) = 0;
	virtual EResult AddPhysicalEntity(IEntity** pEntity) = 0;
};





// Scripting

struct IScript : public virtual INamed
{







	//	.	.	.	.	.	.	.	.







};





////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////








//	I M P L E M E N T A T I O N 
//	(not accessible by application!)









////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class FrameEngine : IFrameEngine
{
public:
	virtual EResult Initialize();
	virtual EResult Clear();

	virtual EResult BeginFrame();
	virtual EResult EndFrame();
};





// Scripting:



class EntityClass : IEntity, Named
{
	virtual void SetScript(IScript* pScriptInstance);
	virtual IScript* GetScript();
	virtual EResult ExecuteScript();
};

class Script : Named
{

};