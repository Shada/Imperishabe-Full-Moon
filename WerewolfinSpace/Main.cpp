

#include "DxHandler.h"
#include "InputHandler.h"
#include "SoundHandler.h"
#include "Player.h"
#include "Game.h"

HINSTANCE				g_hInst					= NULL;  
HWND					g_hWnd					= NULL;

DxHandler*				dxHandler               = NULL;
InputHandler*			inputHandler			= NULL;

Camera*					camera					= NULL;
Game*					game					= NULL;

HRESULT             InitWindow( HINSTANCE hInstance, int nCmdShow );
LRESULT CALLBACK	WndProc( HWND, UINT, WPARAM, LPARAM);
void				CleanupDevice();


int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
		return 0;
	
	camera = new Camera();
	ShowCursor(false);
	
	SetCursorPos(500, 500);
	SetWindowPos(g_hWnd, HWND_TOP, 0, 0, SCREENWIDTH, _SCREENHEIGHT + 32, SWP_SHOWWINDOW);

	dxHandler = new DxHandler(g_hWnd, camera);

	dxHandler->clearRenderTarget();
	
	inputHandler = new InputHandler(hInstance, &g_hWnd);

	game = new Game(dxHandler,inputHandler,camera);

	__int64 currTimeStamp = 0, prevTimeStamp, cntsPerSec = 0;
	
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	double dt, time = 0;
	double secsPerCnt = 1.0 / (double)cntsPerSec;
	
	
	QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
	prevTimeStamp = currTimeStamp;

	// Main message loop
	MSG msg = {0};
	while(WM_QUIT != msg.message)
	{
		prevTimeStamp = currTimeStamp;
		
		QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);

		dt = (currTimeStamp - prevTimeStamp) * secsPerCnt;

		time += dt;

		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else if(time >= (1.f / 60.f))
		{
			
			if(GetActiveWindow() == g_hWnd)
			{
				dxHandler->Update();
				inputHandler->Update();
				game->Update(time);

				SetCursorPos((int)(SCREENWIDTH * 0.5f), (int)(_SCREENHEIGHT * 0.5f));
			}
			else
			{
				QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
				prevTimeStamp = currTimeStamp;
			}

			
			dxHandler->clearRenderTarget();
			game->Draw(time);
			
			dxHandler->swapChain();
			time = 0;
		}
		 
		
	}

	CleanupDevice();
	_CrtDumpMemoryLeaks();
	return (int) msg.wParam;
}

HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = 0;
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = "WiS";
	wcex.hIconSm        = 0;
	if( !RegisterClassEx(&wcex) )
		return E_FAIL;

	// Create window
	g_hInst = hInstance; 
	RECT rc = { 0, 0, SCREENWIDTH, _SCREENHEIGHT };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );

	if(!(g_hWnd = CreateWindow(	"WiS",
							"Imperishable Full Moon",
							WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							rc.right - rc.left,
							rc.bottom - rc.top,
							NULL,
							NULL,
							hInstance,
							NULL)))
	{
		return E_FAIL;
	}

	ShowWindow( g_hWnd, nCmdShow );

	return S_OK;
}
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void CleanupDevice()
{
	SAFE_DELETE( camera );

	delete dxHandler;
	delete inputHandler;
	delete game;

	dxHandler = NULL;
	inputHandler = NULL;
	game = NULL;
	
}