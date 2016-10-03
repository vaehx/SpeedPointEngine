
//
// SpeedPoint Engine Example
//

#include <SpeedPointEngine.h>
#include <Implementation\DirectX11\DirectX11Renderer.h>


class Application : public IApplication
{
private:
  IFramePipeline* m_pFramePipeline;
  IGameEngine* m_pEngine;
  HWND m_hWnd;  
public:
  void SetHWND(HWND hWnd)
  {
    m_hWnd = hWnd;  
  }  
  HWND GetHWND()
  {
    return m_hWnd;  
  }

  virtual void OnInit(IFramePipeline* pFramePipe, IGameEngine* pGameEngine);
  virtual void OnLogReport(SResult res, const char* msg);
  virtual void OnSettingChange(const SSettingsDesc& dsc);
  
  virtual void Update(float fLastFrameTime);
  virtual void Render();      
};

void Application::OnInit(IFramePipeline* pFramePipe, IGameEngine* pGameEngine)
{
  m_pFramePipeline = pFramePipe;
  m_pEngine = pGameEngine;  
}

void Application::OnLogReport(SResult res, const char* msg)
{
  if (Failure(res))
    MessageBoxA(msg, "Error", MB_ICONERROR | MB_OK);
  else
    MessageBoxA(msg, "Info", MB_ICONINFORMATION | MB_OK); 
}

void Application::OnSettingChange(const SSettingsDesc& dsc)
{
  // there are no important settings yet we could handle
}

void Application::Update(float fLastFrameTime)
{
  // DoPlayerControl(fLastFrameTime);
  
  // After this function, the engine computes physics, AI and entity scripting    
} 

void Application::Render()
{
  
}










LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
  if (iMessage == WM_QUIT)
    DestroyWindow(hWnd);
  else if (iMessage == WM_CLOSE)
    PostQuitMessage(0);
  else
    return DefWindowProc(hWnd, iMessage, wParam, lParam);
    
  return 0;        
}

void InitWindow(HWND* phWnd, HINSTANCE hInstance, unsigned int winWidth, unsigned int winHeight)
{
  WNDCLASS wc;
  ZeroMemory(&wc, sizeof(WNDCLASS));
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.hCursor = (HCURSOR)LoadCursor(0, IDC_ARROW);
  wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
  wc.hInstance = hInstance;
  wc.lpfnWndProc = WndProc;
  wc.lpszClassName = L"SPEXAMPLEAPP";
  wc.style = CS_HREDRAW | CS_VREDRAW;
  if (!RegisterClass(&wc)
    return;

  *phWnd = CreateWindow(wc.lpszClassName,
    L"SpeedPoint Example Application",
    WS_VISIBLE | WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDFEAULT,
    winWidth, winHeight,
    nullptr, nullptr, hInstance, 0);
    
  //ShowWindow(hWnd, SW_SHOW);
  UpdateWindow(*phWnd);          
}

void InitEngine(IGameEngine* pEngine, Application* pApplication, unsigned int resX, unsigned int resY)
{
  if (pEngine == nullptr || ppApplication == nullptr)
    return;
      
  pEngine->RegisterApplication(*ppApplication);
  
  SSettingsDesc dsc;
  dsc.app.nXResolution = resX;
  dsc.app.nYResolution = resY;
  dsc.app.hWnd = pApplication->GetHWND();
  dsc.mask = ENGSETTING_RESOLUTION | ENGSETTING_HWND;
  
  pEngine->GetSettings()->Set(dsc);
  pEngine->InitializeLogger();
  pEngine->InitializeFramePipeline();
  pEngine->InitializeRenderer(S_DIRECTX11, DirectX11Renderer::GetInstance());
  pEngine->InitializeResourcePool();
  pEngine->FinishInitialization(); // calls Application::OnInit      
}

void DoMainLoop(IGameEngine* pGameEngine)
{
  bool bRun = true;
  bool bPMResult;
  MSG msg;
  ZeroMemory(&msg, sizeof(MSG));
  while(bRun)
  {    
    if ((bPMResult = PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE)) != 0)
    {
      if (bPMResult == -1)
      {        
        break; // error in PM                     
      }
      else
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT)
          bRun = false;      
      }
    }
    else
    {
      // Tick engine frame
      pEngine->ExecuteFramePipeline();     
    }            
  }
}



void main()
{
  HWND hWnd;  
  unsigned int winWidth = 1280, winHeight = 1024;
  
  // The main objects are created on stack
  // If you want to use another framework you can also create it on heap but should
  // make sure, that the objects are properly destructed!  
  SpeedPointEngine engine;
  Application application;  
  
  InitWindow(&hWnd, GetModuleHandle(nullptr), winWidth, winHeight);
  application.SetHWND(hWnd);
  
  // InitEngine(&engine, &application, winWidth, winHeight);
  // |-engine.FinishInitialization();
  // | |-application.OnInit();
  // | | |-m_pEngine = &engine;
  // ----- 
  InitEngine(&engine, &application, winWidth, winHeight);  
  
  DoMainLoop(&engine);
               
}
