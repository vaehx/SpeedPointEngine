

----------------------------------------------------------------------------------------------
				1. Application Framework

Create your own EngineApplication class here that overrides SpeedPointApplication or
implements SpeedPoint::IApplication::OnInitGeometry():

class EngineApplication : public SpeedPoint::SpeedPointApplication
{
private:
	SpeedPoint::SpeedPointEngine engine;
public:
	void Start();

	void OnRender();
	void OnUpdate();
	....
	????????
};

void EngineApplication::Start()
{
	SEngineStartDesc startDsc;

	// should be true whenever you also need the normal message pump without delay due to rendering
	startDsc.bUseExtraThread = true;
	startDsc.pRenderer = SpeedPoint::DirectX11Renderer::GetInstance();

	engine.Initialize( ????? );
	

	...


	
}

void main()
{
	EngineApplication app;
	app.Start();	

	while(true)
	{
		// Do main loop here. Engine has its own thread
	}
}



----------------------------------------------------------------------------------------------
				2. Initializing Objects

Override SpeedPointApplication::OnInitScene() or implement
SpeedPoint::IApplication::OnInitScene() to do so:

class EngineApplication : ...
{
...
public:
	void OnInitScene();
...
};

void EngineApplication::OnInit()
{
	// Creation of an object with given geometry information:
	SInitialGeometryDesc initGeomDsc;
	initGeomDsc.pVertices = new SVertex[]
	{
		SpeedPoint::Vertex(x,y,z,	nx,ny,nz,	tu,tv),
		SpeedPoint::Vertex(x,y,z,	nx,ny,nz,	tu,tv),
		SpeedPoint::Vertex(x,y,z,	nx,ny,nz,	tu,tv)
	};

	SpeedPoint::StaticObject so;
	so.Init(engine.GetRenderer(), initGeomDsc);



	// Todo:
	// 	- Material for whole object
	//	- Geometry partitioning + separate material for each partition


}