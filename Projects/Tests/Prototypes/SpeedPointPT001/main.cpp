/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Prototype Test Project Source File
//
//	Remember to be frugal with log calls, as SpeedPoint already throws a huge amount of them!
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// DO NOT INCLUDE SPEEDPOINT STUFF HERE! DO THAT IN TEST.CPP INSTEAD!
#include "TestUtil.h"
#include "Test.h"
#include <Windows.h>

/////////////////////////////////////////////////////////////////////////////////////////////////

// Window Callback processing function
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_QUIT:
		DestroyWindow(hWnd);
		break;

	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, iMessage, wParam, lParam);
	}	

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

// win32 console entry
int main(void)
{
	bool bSuccess = true;
	HINSTANCE hInstance = GetModuleHandle(0);
	HWND hWnd = 0;

	// Register window class
	WNDCLASS wc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hCursor = (HCURSOR)LoadCursor(0, IDC_ARROW);
	wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = L"SPPT";
	wc.lpszMenuName = 0;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	if (!RegisterClass(&wc))
		bSuccess = false;
	
	// Create the window
	if (bSuccess)
	{
		if (!(hWnd = CreateWindow(wc.lpszClassName,
			L"SpeedPoint Prototype Test Application",
			WS_VISIBLE | WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			1280, 768,
			0,
			0,
			hInstance,
			0)))
		{
			bSuccess = false;
		}		
	}

	// Update window
	if (bSuccess && FAILED(UpdateWindow(hWnd))) 
		bSuccess = false;

	// Run Message queue if not failed til now
	if (!bSuccess)
	{
		std::cout << "Failed create window!";
	}
	else
	{
		Test t;
		bSuccess = t.Start(hWnd, hInstance); // Start the engine 
		if (bSuccess)
		{
			bool bAppRunning = true;
			bool bRet;
			MSG msg;
			ZeroMemory(&msg, sizeof(MSG));

			while (bAppRunning)
			{
				if (GetAsyncKeyState(VK_ESCAPE) < 0)
				{
					bAppRunning = false;					
				}

				if ((bRet = PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE)) != 0)
				{
					if (bRet == -1)
					{
						// error!
						std::cout << "Fatal error in msg queue";
						break;
					}					
					else
					{
						TranslateMessage(&msg);												
						DispatchMessage(&msg);
						if (msg.message == WM_QUIT)
						{
							bAppRunning = false;							
						}
					}					
				}
				else
				{
					// Do a tick
					if (!t.Tick())
					{
						std::cout << "ERROR OCCURED DURING TICK CALL!" << std::endl;
						bSuccess = false;
						PostQuitMessage(0);
					}
				}
			}

			// Stop the engine, no matter if anything failed or ont
			t.Stop();
		}

		DestroyWindow(hWnd);
	}

	// Take a single character as input to end the program application
	TestUtil::EndInp();
	return true;
}