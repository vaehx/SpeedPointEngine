// only stores data for renderable, no functionality
struct IRenderableComponent
{
	virtual Geometry* GetGeometry() = 0;
	virtual Material** GetMaterials() = 0;
	virtual unsigned int GetMaterialCount() const = 0;
};
class RenderableComponent
{
private:
	Geometry geometry;
	Material** pMaterials;
	unsigned int nMats; // count of material pointers
public:
	virtual Geometry* GetGeometry() { return &geometry; }
	virtual Material** GetMaterials() { return pMaterials; }
	virtual unsigned int GetMaterialCount() const { return nMats; };
};



struct IStatic
{
	virtual IRenderableComponent* GetRenderable() = 0;
};
class CStatic
{
	...
};


// scriptable entity
struct IEntity
{
	virtual IRenderableComponent* GetRenderable() = 0;
	virtual IPhysicalComponent* GetPhysical() = 0;
	virtual IScriptableComponent* GetScriptable() = 0;
	...	
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



struct IRenderSchedule
{
};