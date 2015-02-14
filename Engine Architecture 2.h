typedef class Geometry Mesh;


class CRenderableComponent : public IRenderableComponent
{
private:
	Geometry geometry;
	Material** pMaterials; // array of pointers to materials lying in a material pool
	unsigned int nMats; // count of material pointers
public:
	virtual Geometry* GetGeometry() { return &geometry; }
	virtual Material** GetMaterials() { return pMaterials; }
	virtual unsigned int GetMaterialCount() const { return nMats; };
};


// ------------------------------------------------------------------------------

class PhysicalObjectComponent : public IPhysicalObjectComponent
{
private:
	// physical object has no geometry, material, etc. and is not renderable.
	// It's only the plain physical object

	CollisionShape collisionShape;
	Velocity velocity;
	float mass;
	EPhysicalBeheaviour beheaviour;
	...
public:
	// implement IPhysicalObjectComponent here.
	...
};

// ------------------------------------------------------------------------------

class AnimateableObjectComponent : public IAnimateableObjectComponent
{
private:
	Geometry* pGeometry; // accumulation
	AnimationKey* pKeys; // composition
	unsigned int nAnimationKeys;
	float animationRunTime;
	AnimBoneMapping boneMapping;
    ...

public:	
	// implement IAnimateableObjectComponent...
};

// ------------------------------------------------------------------------------
struct IScriptObjectComponent
{
	virtual Script* GetScript() = 0;
	virtual ScriptEnvironment* GetScriptEnvironment() = 0;
};
class ScriptObjectComponent : public IScriptObjectComponent 
{
private:
	Script script;
	ScriptEnvironment scriptEnv;
public:
	// implement IScriptObjectComponent...
};






struct IObject : public STransformable
{
    virtual bool IsRenderable() const { return false; }
    virtual bool IsPhysical() const { return false; }
    virtual bool IsAnimateable() const { return false;}
    virtual bool IsScriptable() const { return false; }
    
    virtual IRenderableComponent* GetRenderable() { return 0; }
    virtual IPhysicalComponent* GetPhysical() { return 0; }
    virtual IAnimateableComponent* GetAnimateable() { return 0; }
    virtual IScriptableComponent* GetScriptable() { return 0; }    
};

struct IStatic;     // no physics, just renderable
struct IStaticPhysical; // rigid body physics, renderable
struct ICharacter; // rigid body physics, renderable, skinning information
struct IEntity; // scriptable, everything else is optional

struct IVegetable; // vegetation physics, geometry, vegetation information

struct IWater; // water-physics, renderable
struct ICloth; // cloth-physics, renderable

...




struct IStatic : public IObject
{
    virtual bool IsRenderable() const { return true; }    
    
	//virtual IRenderableComponent* GetRenderable();      
};
class CStatic : public IStatic
{
private:
    CRenderableComponent renderable;
public:
    virtual IRenderableComponent* GetRenderable() { return (IRenderableComponent*)&renderable; }
};


// scriptable entity
struct IEntity : public IObject
{
};
class CEntity
{
private:
    IRenderableComponent* pRenderable;
    IPhysicalComponent* pPhysical;
    IAnimateableComponent* pAnimateable;
    IScriptableComponent scriptable;
public:
    virtual bool IsRenderable() const { return IS_VALID_PTR(pRenderable); }
    virtual bool IsPhysical() const { return IS_VALID_PTR(pPhysical); }
    virtual bool IsAnimateable() const { return IS_VALID_PTR(pAnimateable); }
    virtual bool IsScriptable() const { return true; }
    
    virtual IRenderableComponent* GetRenderable() { return pRenderable; }
    virtual IPhysicalComponent* GetPhysical() { return pPhysical; }
    virtual IAnimateableComponent* GetAnimateable() { return pAnimateable; }
    virtual IScriptableComponent* GetScriptable() { return (IScriptableComponent*)&scriptable; }     
};



struct SDrawDesc
{
	IVertexBuffer* pVB;
	IIndexBuffer* pIB;
	usint32 iStartIBIndex,
		iEndIBIndex,
		iStartVBIndex,
		iEndVBIndex;

	STransformationDesc transform;
	enum EPrimitiveType {
		ePRIMITIVETYPE_TRIANGLES,
		ePRIMITIVETYPE_LINES
	} primitiveType;
};
struct SRenderDesc
{
	enum EDrawPipeline {
		eDRAWPIPE_FORWARD, eDRAWPIPE_DEFERRED
	} drawPipeType;

	SDrawDesc drawDesc;
	IGeometry* pGeometry;
	SMaterial material;
};

struct SRenderScheduleSlot
{
	SRenderDesc renderDesc;	
};