// road_editor.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "road_editor.h"
#include "TimeUtility.h"
#include "Renderer.h"
#include "Application.h"

#define MAX_LOADSTRING 100

//anonymous namespace variables
namespace
{
	//TIME CONSTANTS
	const double DESIRED_FRAME_RATE = 60.0;
	const gh::SystemClocks DESIRED_CLOCKS_PER_FRAME = gh::SecondsToClocks( 1.0 / DESIRED_FRAME_RATE );

	// Global Variables:
	HINSTANCE hInst;								// current instance
	TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
	TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

	//pointer to the application
	gh::Application* g_pApp = nullptr;
}

//functions for starting open gl
HGLRC				InitializeOpenGL(HDC);
void				ShutdownOpenGL(HGLRC&);

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
HWND				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	gh::SystemClocks nextFrameTime = gh::GetAbsoluteTimeClocks();

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_ROAD_EDITOR, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	HWND handleToWindow = InitInstance(hInstance, nCmdShow);
	if (handleToWindow == nullptr)
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ROAD_EDITOR));

	//setup openGL rendering context
	HDC hDC = GetDC(handleToWindow);
	HGLRC hGLRenderingContext = InitializeOpenGL(hDC);

	//retrieve window dimensions
	RECT clientRect;
	GetClientRect(handleToWindow, &clientRect);

	//initialize application and resize
	g_pApp = new gh::Application(handleToWindow, hDC);
	g_pApp->onResize(clientRect.right, clientRect.bottom);

	// Main message loop:
	bool keepRunning = true;
	while (keepRunning)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				//quit the application
				keepRunning = false;
			}

			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gh::GetAbsoluteTimeClocks() >= nextFrameTime)
			{
				nextFrameTime = gh::GetAbsoluteTimeClocks() + DESIRED_CLOCKS_PER_FRAME;
				g_pApp->updateFrame(gh::ClocksToSeconds(DESIRED_CLOCKS_PER_FRAME));
			}
		}
	}

	ShutdownOpenGL(hGLRenderingContext);
	ReleaseDC(handleToWindow, hDC);

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ROAD_EDITOR));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (hWnd)
   {
	   ShowWindow(hWnd, nCmdShow);
	   UpdateWindow(hWnd);
   }

   return hWnd;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_SIZE:
		// Enforce the required aspect ratio
		//
		g_pApp->onResize( LOWORD( lParam ), HIWORD( lParam ) );
		break;

	case WM_LBUTTONUP:
		{
			Vector2 mouseLocation( LOWORD(lParam), HIWORD(lParam) );
			mouseLocation.y = theRenderer.getWindowSize().y - mouseLocation.y;
			g_pApp->readMouseClick( mouseLocation );
		}
		break;

	case WM_RBUTTONUP:
		{
			//gh::ProfileClock::hideProfileStatistics("testProfile");
		}
		break;

	case WM_SETFOCUS:
		//ShowCursor(false);
		g_pApp->updateInput( true );
		break;

	case WM_KILLFOCUS:
		//ShowCursor(true);
		g_pApp->updateInput( false );
		break;

	case WM_KEYDOWN:
		g_pApp->keyPressed( wParam );
		//g_pApp->aSyncKeyPressed();
		break;

	case WM_KEYUP:
		g_pApp->KeyReleased( wParam );
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

HGLRC InitializeOpenGL( HDC hdcWindow )
{
	assert( hdcWindow );

	// Set the window pixel format
	//
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor = {0};

	pixelFormatDescriptor.nSize = sizeof( pixelFormatDescriptor );
	pixelFormatDescriptor.nVersion = 1;

	pixelFormatDescriptor.dwFlags = 
		PFD_DRAW_TO_WINDOW | 
		PFD_SUPPORT_OPENGL | 
		PFD_DOUBLEBUFFER;
	pixelFormatDescriptor.dwLayerMask = PFD_MAIN_PLANE;
	pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	pixelFormatDescriptor.cColorBits = 32;
	pixelFormatDescriptor.cDepthBits = 16;

	int pixelFormat = ChoosePixelFormat( hdcWindow, &pixelFormatDescriptor );
	assert (pixelFormat != 0);				// This means that the system cannot find a pixel format that can conform to it
	SetPixelFormat( hdcWindow, pixelFormat, &pixelFormatDescriptor );

	// Create the OpenGL render context
	//
	HGLRC renderingContext = wglCreateContext( hdcWindow );
	wglMakeCurrent( hdcWindow, renderingContext );

	glEnable( GL_DEPTH_TEST );

	return renderingContext;
}

void ShutdownOpenGL( HGLRC& renderingContext )
{
	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( renderingContext );
	renderingContext = NULL;
}