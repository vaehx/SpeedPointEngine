
namespace SpeedPoint
{
    // Engine components:
	struct IRenderer;
    struct IResourcePool;
	struct ISceneManager;
	struct IGameEngine;
}



#define CLEAR_DESTRUCT(x) if (IS_VALID_PTR((x))) { x->Clear(); delete x; x = 0; }



// Memory: [object*, object*, object*] ... [object] ... [object] ... [object] ... [object] ... [object] ... [object]
// Problem: probably intense memory defrag

// Memory: [objectlist*, objectlist*] ... [object, object, object] ... [object, object, object]
// Less defrag, but objects in one list have to be from same type







// Use components:






enum EObjectType
{
    eOBJ_STATIC,
    eOBJ_STATIC_PHYSICAL,
    eOBJ_CHARACTER,
    eOBJ_ENTITY
};

struct IObject : public Transformable 
{    
    virtual bool IsRenderable() const { return false; }
	virtual bool IsPhysical() const { return false; }
	virtual bool IsAnimateable() const { return false; }
	virtual bool IsScriptable() const { return false; }

	virtual IRenderableObjectComponent* GetRenderable() { return 0; }
	virtual IPhysicalObjectComponent* GetPhysical() { return 0; }	
	virtual IAnimateableObjectComponent* GetAnimateable() { return 0; }
	virtual IScriptObjectComponent* GetScriptable() { return 0; }                        
};

struct IRenderableObject : public IObject
{
	virtual Geometry* GetGeometry() = 0;
	virtual Material** GetMaterials() = 0;
    virtual SResult Render() = 0;

	virtual bool IsRenderable() const { return true; }
};

// ------------------------------------------------------------------------------
struct IStaticObject : public IRenderableObject
{
    virtual ~IStaticObject() {}
	virtual IRenderableObjectComponent* GetRenderable();
};
class StaticObject : public IStaticObject
{
private:
	RenderableObjectComponent renderable;

// IStaticObject:
public:
    virtual ~StaticObject()
    {
        renderable.Clear();
    }
    
    virtual IRenderableObjectComponent* GetRenderable() { return &renderable; }

// IRenderableObject:
public:
	virtual Geometry* GetGeometry() { return renderable.GetGeometry(); }
	virtual Material** GetMaterials() { return renderable.GetMaterials(); }
    
    virtual SResult Render();
};

// ------------------------------------------------------------------------------
struct IPhysicalObject : public IObject
{	
    virtual ~IPhysicalObject() {}
};
class PhysicalObject : public IPhysicalObject
{
private:
	RenderableObjectComponent renderable;
	PhysicalObjectComponent physical;
public:
    virtual ~PhysicalObject()
    {
        renderable.Clear();
        physical.Clear();    
    }
};

// ------------------------------------------------------------------------------
struct ICharacter : public IObject
{
    virtual ~ICharacter() {}
    virtual void Create() = 0;
};
class Character : public ICharacter
{
private:
	RenderableObjectComponent renderable;
	AnimateableObjectComponent animateable;

// ICharacter:
public:
    virtual ~Character()
    {
        renderable.Clear();
        animateable.Clear();    
    }            
	virtual void Create()
	{
		renderable.Create();
		animateable.Create(renderable.GetGeometry());
	}
    
// IRenderableObject:
public:
    virtual Geometry* GetGeometry() { return renderable.GetGeometry(); }
	virtual Material** GetMaterials() { return renderable.GetMaterials(); }

	virtual IRenderableObjectComponent* GetRenderable() { return &renderable; }
    
    virtual SResult Render();	
};

// ------------------------------------------------------------------------------
struct IEntity : public IObject
{
    virtual ~IEntity() {}
};

// entity is component based, so can be instanciated.
// Components get destructed in Entity::~Entity() if valid pointer
class Entity : public IEntity
{
private:
	RenderableObjectComponent* pRenderable;	// composition, optional
	PhysicalObjectComponent* pPhysical; // composition, optional
	AnimateableObjectComponent* pAnimateable; // composition, optional
	ScriptObjectComponent* pScript;
    
public:
    virtual ~Entity()
    {
        CLEAR_DESTRUCT(pRenderable);
        CLEAR_DESTRUCT(pPhysical);
        CLEAR_DESTRUCT(pAnimateable);
        CLEAR_DESTRUCT(pScript);
    }
};

// ------------------------------------------------------------------------------
struct ISky : public IObject
{
};
class CSky : public ISky
{
private:
    RenderableObjectComponent* pRenderable;
public:
};














// ---------------------------------------------------------------------------------------------------

struct SLightGeomStorage
{
    IGeometry* pOmnidirectional;    // sphere
    IGeometry* pSpot;   // cone
    
    SLightGeomStorage()
        : pOmnidirectional(0),
        pSpot(0) {}
        
    ~SLightGeomStorage()
    {
        if (IS_VALID_PTR(pOmnidirectional))
            delete pOmnidirectional;
            
        if (IS_VALID_PTR(pSpot))
            delete pSpot;
            
        pOmnidirectional = 0;
        pSpot = 0;    
    }       
};

enum ELightType
{
    eLIGHT_DIRECTIONAL,
    eLIGHT_OMNIDIRECTIONAL,
    eLIGHT_SPOT,
    eLIGHT_AREAL    
};

struct SLight
{
    ELightType type;
    SVector3 position;
    SVector3 direction;
    SColor color;
    float intensity;
    SVector3 scale;   
    
    IGeometry* pGeometry;             
    SMatrix transform;    
    
    SLight()
        : type(eLIGHT_DIRECTIONAL),
        pGeometry(0),
        intensity(0.0f) {}                                          
};


enum EShaderType
{
	eSHADERTYPE_IDEAL_DIFFUSE,	// Lambert only
	eSHADERTYPE_BLINNPHONG,		// lambert + blinn specular	
	eSHADERTYPE_COOKTORRANCE	// cook torrance model w/ schlick approximation of fresnel term
};

typedef ITexture* TextureResource;
struct Material
{
	EShaderType shaderType;
	TextureResource textureMap;
	TextureResource normalMap;
	float specularExponent; // for Blinn-Phong
	float refractiveIndex; // for cook torrance fresnel term
	float specularity;	
};

struct IOctree
{
    virtual ~IOctree() {}

    virtual bool Insert(IObject* obj, const unsigned int maxDepth = DEF_MAX_DEPTH, const unsigned int curDepth = 0) = 0;
    virtual void GetIntersectingOctrees(std::vector<IOctree*>& out, const AABB& aabbInt) = 0;
    virtual void Clear() = 0;     
};
class Octree : public IOctree
{
private:    
    Octree* subTrees; // do not use fixed size array here - endless destruction!
    Octree* pParent;    
    bool isLeave;
    ChunkPool<IObject*> objects;
    AABB aabb;
    
    enum { DEF_MAX_DEPTH = 5 };
    
public:
    Octree()
        : isLeave(false),
        nSubTrees(0) {}        
    virtual ~Octree()
    {        
        Clear();                
    }        
    
    // return true if inserted
    virtual bool Insert(IObject* obj, const unsigned int maxDepth = DEF_MAX_DEPTH, const unsigned int curDepth = 0)
    {
        if (!IS_VALID_PTR(obj))
            return;
            
        if (!aabb.Intersects(obj->GetAlixAlignedBoundBox()))
            return false;
            
        if (curDepth == maxDepth)
        {
            isLeave = true;
            objects.Add(obj);
            return true;         
        }        
        
        if (isLeave)
        {
            // create suboctrees
            subTrees = new Octree[8];
            for (unsigned int i = 0; i < 8; ++i)
                subTrees[i].pParent = this;
                
            SVector3 vMaxH = aabb.vMax * 0.5f;
            subTrees[0].aabb = AABB(aabb.vMin, vMaxH);  // left bottom front
            subTrees[1].aabb = AABB(SVector3(vMaxH.x, aabb.vMin.y, aabb.vMin.z), SVector3(aabb.vMax.x, vMaxH.y, vMaxH.z);   // right bottom front
            subTrees[2].aabb = AABB(SVector3(aabb.vMin.x, aabb.vMin.y, vMaxH.z), SVector3(vMaxH.x, vMaxH.y, aabb.vMax.z));  // left bottom back
            subTrees[3].aabb = AABB(SVector3(vMaxH.x, aabb.vMin.y, vMaxH.z), SVector3(aabb.vMax.x, vMaxH.y, aabb.vMax.z)); // right bottom back
            subTrees[4].aabb = AABB(SVector3(aabb.vMin.x, vMaxH.y, aabb.vMin.z), SVector3(vMaxH.x, aabb.vMax.y, vMaxH.z)); // left top front
            subTrees[5].aabb = AABB(SVector3(vMaxH.x, vMaxH.y, aabb.vMin.z), SVector3(aabb.vMax.x, aabb.vMax.y, vMaxH.z)); // right top front
            subTrees[6].aabb = AABB(SVector3(aabb.vMin.x, vMaxH.y, vMaxH.z), SVector3(vMaxH.x, aabb.vMax.y, aabb.vMax.z)); // left top back
            subTrees[7].aabb = AABB(vMaxH, aabb.vMax); // right top back
                        
            isLeave = false;        
        }
                        
        for (unsigned int i = 0; i < 8; ++i)
        {
            if (subTrees[i].Insert(obj, maxDepth, curDepth + 1))
                return true;            
        }
        
        return false;                                                                                                                                     	                        
    }
    
    // This function uses recursive DFS algorithm.
    // Note: if an object intersects with multiple Octrees, all of them are added to
    // the output.
    virtual void GetIntersectingOctrees(std::vector<IOctree*>& out, const AABB& aabbInt)
    {
        if (isLeave || !IS_VALID_PTR(subTrees))
        {
            if (aabb.Intersects(aabbInt))
                out.push_back(this);
                
            return;                                        
        }                
                                                     
        for (unsigned int i = 0; i < 8; ++i)
        {
            subTrees[i].GetIntersectingOctrees(out, aabbInt);              
        }                
    }
    
    virtual void Clear()
    {
        isLeave = false;
        objects.Clear();
        if (!isLeave && IS_VALID_PTR(subTrees))
            delete[] subTrees;
        
        pParent = 0;  
    }                               
};

struct IScene
{
    virtual ~IScene() {}

    virtual void AddObject(IObject* pObject) = 0;
    virtual IObject* CreateObject(const EObjectType type) = 0;
    
    virtual IOctree* GetOctree() = 0;    
    virtual ITerrain* GetTerrain() = 0;        
};
// When destructed, all object pointers that appear still valid get deleted.
// Pay attention that you set them to 0 if you destruct them yourself, otherwise
// memory access violation occurs!
class Scene : public IScene
{
private:
	ChunkPool<IObject*> m_Objects;
    ChunkPool<SLight> m_Lights;
    IOctree* m_pOctree; // comp
    ITerrain* m_pTerrain; // comp
    //ISkyBox* m_pSkyboxM
    
public:
    virtual ~Scene()
    {
        CLEAR_DESTRUCT(m_pOctree);
        CLEAR_DESTRUCT(m_pTerrain);
        m_Objects.Clear();
        m_Lights.Clear();    
    }

    virtual void AddObject(IObject* pObject)
    {
        m_Objects.Add(pObject);
        m_Octree.Insert(pObject);    
    }        
    
    // Create a standard engine object. If you want to create an object of
    // own implementation of IObject, instanciate it/create on heap yourself
    // and pass as IObject* to AddObject(); 
    virtual IObject* CreateObject(const EObjectType type)
    {
        IObject* pInstance = 0;
        switch(type)
        {
        case eOBJ_STATIC: pInstance = new StaticObject(); break;        	    
        case eOBJ_STATIC_PHYSICAL: pInstance = new PhysicalObject(); break; 
        case eOBJ_CHARACTER: pInstance = new Character(); break;
        case eOBJ_ENTITY: pInstance = new Entity(); break;                    
        }
        
        if (!IS_VALID_PTR(pInstance))
            return 0;
            
        m_Objects.Add(pInstance);        
    }                                    
};

struct ISceneManager
{
    virtual IScene* GetPrimaryScene() = 0;
};
class SceneManager : public ISceneManager
{
private:
	Scene* primaryScene;
public:
    virtual IScene* GetPrimaryScene() { return (IScene*)primaryScene; }    
};




struct IGameEngine
{
    ...
    virtual ISceneManager* GetSceneManager() = 0;
    virtual SLightGeomStorage* GetLightGeomStorage() = 0;
    ...
}:

class SpeedPointEngine : public IGameEngine
{
private:
    ...
    SLightGeomStorage m_LightGeomStorage;
    ...
public:
    ...
    virtual SLightGeomStorage* GetLightGeomStorage() { return &m_LightGeomStorage; }
    ...
};








========================================================================================================


// Summary:
//	This is an adapter to ChunkPool, renaming the functions of it.
template<typename T, unsigned int chunkSize>
class ChunkedObjectPool
{
private:
	ChunkPool<T, chunkSize> instances;
	
public:
	ChunkedObjectPool()
};