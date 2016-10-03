struct SSceneNode;

struct IObject
{
    SSceneNode* pNode;
};


struct SSceneNode
{
	IObject* pObject;
	AABB aabb; 
    
    SSceneNode()
        : pObject(0) {}
        
    SSceneNode(const SSceneNode& node)
        : pObject(node.pObject),
        aabb(node.aabb) {}
        
    ~SSceneNode()
    {
        if (IS_VALID_PTR(pObject))
        {
            if (pObject->GetSceneNode() == this)
                pObject->SetSceneNode(0);
                
            pObject = 0;
        }                            
    }
    
    SSceneNode& operator = (const SSceneNode& node)
    {
        pObject = node.pObject;
        aabb = node.aabb;
        return *this;    
    }        
};

struct SSceneGraphQueryInfo
{
	SSceneNode* pNodes;
	unsigned int nNodes;

	SSceneGraphQueryInfo()
		: pNodes(0), nNodes(0) {}

	~SSceneGraphQueryInfo()
	{
		if (IS_VALID_PTR(pNodes))
			delete[] pNodes;

		nNodes = 0;
		pNodes = 0;
	}
};

// Scene graph can have different implementations.
// For example a simple list or an octree
struct ISceneGraph
{
    virtual ~ISceneGraph()
    {    
    }
    
	virtual void Insert(const SSceneNode& node) = 0;
	virtual void QueryIntersectingNodes(const AABB& aabb, SSceneGraphQueryInfo* pInfo) = 0;
	virtual void Remove(IObject** pObject) = 0;    
};

static ISceneGraph* BuildOctreeSceneGraph();
//static ISceneGraph* BuildListSceneGraph(); 

struct IScene
{
    virtual ~IScene() {}        
    virtual void QueryIntersectingSceneNodes(const AABB& aabb, SSceneGraphQueryInfo* pInfo) = 0;
    
    virtual void AddObject(IObject* pObject) = 0;
    virtual IObject* CreateObject(const EObjectType type) = 0;
       
    virtual ITerrain* GetTerrain() = 0; 
};





// ---
class COctreeSceneGraphTree
{
private:
    COctreeSceneGraphTree* pSubtrees;
    SSceneNode* pSceneNodes;
    unsigned int nSceneNodes;
    AABB aabb;
public:
    COctreeSceneGraphTree()
        : pSubtrees(0),
        pSceneNodes(0),
        nSceneNodes(0) {}
        
    ~COctreeSceneGraphTree()
    {
        Clear();
    }        

    void Insert(const SSceneNode& node, unsigned int curDepth = 0, unsigned int maxDepth = 5)
    {
        if (!IS_VALID_PTR(node.pObject))
            return;
    
        if (curDepth == maxDepth)
        {
            // is leave
            pSubtrees = 0;
            
            SSceneNode* pOldSceneNodes = pSceneNodes;
            pSceneNodes = new SSceneNode[nSceneNodes++];
            
            if (IS_VALID_PTR(pOldSceneNodes) && nSceneNodes > 1)
            {
                memcpy(pSceneNodes, pOldSceneNodes, (nSceneNodes - 1) * sizeof(SSceneNode));
                delete[] pOldSceneNodes;            
            }                           
            
            pSceneNodes[nSceneNodes - 1] = node;
            node.pObject->SetSceneNode(&pSceneNodes[nSceneNodes - 1]);             
        }
                                
    }
    
    // returns true if found and erased, false if not
    bool Erase(IObject* pObject)
    {
        if (!IS_VALID_PTR(pObject))
            return;
            
        const AABB& objAABB = pObject->GetAABB();
        if (!aabb.IntersectsWith(objAABB))
            return;
            
        if (!IS_VALID_PTR(pSubtrees)) // is leaf
        {
            if (!IS_VALID_PTR(pSceneNodes))
                return;
        
            for (unsigned int i = 0; i < nSceneNodes; ++i)
            {
                if ()
            }        
        }
        else
        {
            for (unsigned int i = 0; i < )        
        }                         
    }
    
    void Clear()
    {
        if (IS_VALID_PTR(pSubtrees))        
            delete[] pSubtrees;
            
        if (IS_VALID_PTR(pSceneNodes))
            delete[] pSceneNodes;
            
        pSubtrees = 0;
        pSceneNodes = 0;
        nSceneNodes = 0;                      
    }            
};

class COctreeSceneGraph : public ISceneGraph
{
private:
    
public:
    COctreeSceneGraph()
    {
    }                      
    
    virtual ~COctreeSceneGraph() {}

    virtual void Insert(const SSceneNode& node)
    {            
        // TODO: Insert into octree stuff here        
    }
	
    virtual void QueryIntersectingNodes(const AABB& aabb, SSceneGraphQueryInfo* pInfo)
    {
        if (!IS_VALID_PTR(pInfo))
            return;
    
                
    }
	
    virtual void Remove(IObject** pObject)
    {
                
    }
};


// Scene graph using sweep and prune algorithm
class CSPSceneGraph
{
};


// ------
class CScene
{
private:
    ISceneGraph* pSceneGraph;
    ITerrain* m_pTerrain;
public:
    CScene()
    {
        pSceneGraph = BuildOctreeSceneGraph();    
    }
    virtual ~CScene()
    {
        if (pSceneGraph)
            delete pSceneGraph;
            
        pSceneGraph = 0;                    
    }
    
    virtual void QueryIntersectingSceneNodes(const AABB& aabb, SSceneGraphQueryInfo* pInfo)
    {         
        if (!IS_VALID_PTR(pSceneGraph))
            return;
           
        pSceneGraph->QuerIntersectingNodes(aabb, pInfo);
    }
    
    virtual void AddObject(IObject* pObject)
    {
        if (!IS_VALID_PTR(pObject) || !IS_VALID_PTR(pSceneGraph))
            return;
    
        SSceneNode node;
        node.pObject = pObject;
        pSceneGraph->Insert(node);                        
    }
    
    virtual IObject* CreateObject(const EObjectType type)
    {
        IObject* pObject = 0;
        switch (type)
        {
        case eOBJ_STATIC: pObject = new StaticObject(); break;
        case eOBJ_RIGID_PHYSICAL:
        case eOBJ_STATIC_PHYSICAL: pObject = new PhysicalObject(); break;
        default:
            // ???
            return 0;         
        }
        
        AddObject(pObject);
        return pObject;
    }
       
    virtual ITerrain* GetTerrain()
    {
        return pTerrain; 
    }
};