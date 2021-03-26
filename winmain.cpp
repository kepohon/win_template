// ウィンドウズプログラムの雛形 (2021.3.25)
// Copyright (c) 2021 by: Kepohon

#define WIN32_LEAN_AND_MEAN		// インクルードするファイルの個数を減らすための宣言

#include <Windows.h>
#include "menu.h"

// Function prototypes
int		WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
bool	CreateMainWindow(HWND&, HINSTANCE, int);
LRESULT	WINAPI WinProc(HWND, UINT, WPARAM, LPARAM);
bool	AnotherInstance();

// Global variables
HINSTANCE	hInstance;
HDC			hdc;				// handle to device context
TCHAR		ch = ' ';			// character entered
RECT		rect;				// rectangle structure
PAINTSTRUCT ps;					// used in WM_PAINT
bool		vkKeys[256];		// state of virtual keys, false or true

// Game pointer
//Spacewar *spacewar = NULL;
HWND		hwnd = NULL;
const bool	FULLSCREEN = false;
const char	CLASS_NAME[] = "Template";
const char	WIN_TITLE[] = "Template Window";
const int	WIN_WIDTH = 800;
const int	WIN_HEIGHT = 600;

//=============================================================================
// Starting point for a Windows application
//=============================================================================
int WINAPI WinMain(	HINSTANCE hInstance,		// アプリケーションのポインタ
					HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,			// コマンドラインパラメータ
					int nCmdShow				// ウィンドウをどのように表示するかを指示する
)
{
	MSG msg;

	// 二重起動チェック
	if ( AnotherInstance() )
	{
		MessageBox(	NULL,
					TEXT("二重起動はできません"),
					TEXT("警告！"),
					MB_OK
		);
		return false;
	}

	// Create the spacewar, sets up message handler
	//    spacewar = new Spacewar;

	// ウィンドウ作成
	if (!CreateMainWindow(hwnd, hInstance, nCmdShow))
		return 1;
	
	try {
		//        spacewar->initialize(hwnd);     // throws GameError

		// メッセージループ
		int done = 0;
		while (!done)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					done = 1;

				// decode and pass messages on to WinProc
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
				;
			//                spacewar->run(hwnd);    // run the spacewar loop
		}
		//        SAFE_DELETE (spacewar);     // free memory before exit
		return msg.wParam;
	}
	/*
	catch(const GameError &err)
	{
		spacewar->deleteAll();
		DestroyWindow(hwnd);
		MessageBox(NULL, err.getMessage(), "Error", MB_OK);
	}
	*/
	catch (...)
	{
		//        spacewar->deleteAll();
		DestroyWindow(hwnd);
		MessageBox(NULL, "Unknown error occured in spacewar.", "Error", MB_OK);
	}

	//    SAFE_DELETE (spacewar);     // free memory before exit
	return 0;
}

//=============================================================================
// ウィンドウズイベントのコールバック関数　（ウィンドウズから呼び出される）
//=============================================================================
LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	short		nVirtKey;						// virtual-key code 
	const short	SHIFTED = (short)0x8000;
	TEXTMETRIC	tm;								// structure for text metrics 
	DWORD		chWidth = 20;					// width of characters
	DWORD		chHeight = 20;					// height of characters
	int 		id;

	switch (msg)
	{
	case WM_CREATE:
		// get the text metrics
		hdc = GetDC(hwnd);
		GetTextMetrics(hdc, &tm);
		ReleaseDC(hwnd, hdc);
		chWidth = tm.tmAveCharWidth;			// average character width
		chHeight = tm.tmHeight;					// character height
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);						//tell Windows to kill this program
		return 0;

	case WM_KEYDOWN:							// key down
		vkKeys[wParam] = true;
		switch (wParam)
		{
		case VK_SHIFT:							// shift key
			nVirtKey = GetKeyState(VK_LSHIFT);	// get state of left shift
			if (nVirtKey & SHIFTED)				// if left shift
			{
				vkKeys[VK_LSHIFT] = true;
			}
			nVirtKey = GetKeyState(VK_RSHIFT);	// get state of right shift
			if (nVirtKey & SHIFTED)				// if right shift
			{
				vkKeys[VK_RSHIFT] = true;
			}
			break;
			
		case VK_CONTROL:						// control key
			nVirtKey = GetKeyState(VK_LCONTROL);
			if (nVirtKey & SHIFTED)				// if left control
			{
				vkKeys[VK_LCONTROL] = true;
			}
			nVirtKey = GetKeyState(VK_RCONTROL);
			if (nVirtKey & SHIFTED)				// if right control
			{
				vkKeys[VK_RCONTROL] = true;
			}
			break;
		}
		InvalidateRect(hwnd, NULL, TRUE);		// force WM_PAINT
		return 0;
		break;

	case WM_KEYUP:								// key up
		vkKeys[wParam] = false;
		switch (wParam)
		{
		case VK_SHIFT:							// shift key
			nVirtKey = GetKeyState(VK_LSHIFT);
			if ((nVirtKey & SHIFTED) == 0)		// if left shift
			{
				vkKeys[VK_LSHIFT] = false;
			}
			nVirtKey = GetKeyState(VK_RSHIFT);
			if ((nVirtKey & SHIFTED) == 0)		// if right shift
			{
				vkKeys[VK_RSHIFT] = false;
			}
			break;
			
		case VK_CONTROL:						// control key
			nVirtKey = GetKeyState(VK_LCONTROL);
			if ((nVirtKey & SHIFTED) == 0)		// if left control
			{
				vkKeys[VK_LCONTROL] = false;
			}
			nVirtKey = GetKeyState(VK_RCONTROL);
			if ((nVirtKey & SHIFTED) == 0)		// if right control
			{
				vkKeys[VK_RCONTROL] = false;
			}
			break;
		}
		InvalidateRect(hwnd, NULL, TRUE);		// force WM_PAINT
		return 0;
		break;

	case WM_CHAR:								// a character was entered by the keyboard
		switch (wParam)							// the character is in wParam
		{
		case 0x08:			// backspace
		case 0x09:			// tab
		case 0x0A:			// linefeed
		case 0x0D:			// carriage return
		case 0x1B:			// escape
			MessageBeep((UINT)-1);				// beep but do not display
			return 0;
			
		default:								// displayable character
			ch = (TCHAR)wParam;					// get the character
			InvalidateRect(hwnd, NULL, TRUE);	// force WM_PAINT
			return 0;
		}

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);			// get handle to device context
		TextOut(hdc, 0, 0, &ch, 1);				// display the character

		// Display the state of vkKeys array
		// Display 'T' if key is down and 'F' is key is up
		for (int r = 0; r < 16; r++)
		{
			for (int c = 0; c < 16; c++)
			{
				if (vkKeys[r * 16 + c])
				{
					SetBkMode(hdc, OPAQUE);			// opaque text background
					TextOut(hdc, c * chWidth + chWidth * 2, r * chHeight + chHeight * 2, "T ", 2);
				}
				else
				{
					SetBkMode(hdc, TRANSPARENT);	// transparent text background
					TextOut(hdc, c * chWidth + chWidth * 2, r * chHeight + chHeight * 2, "F ", 2);
				}
			}
		}

		EndPaint(hwnd, &ps);
		return 0;
		
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDM_END:
			SendMessage(hwnd, WM_CLOSE, wParam, lParam);
			break;
		}
		break;
	
	case WM_CLOSE:
		id = MessageBox(	hwnd,
							TEXT("終了してもよろしいですか？"),
							TEXT(" 確認 "),
							MB_YESNO | MB_ICONQUESTION
							);
		if (id == IDYES)
		{
			DestroyWindow(hwnd);
		}
		break;
		
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);

	}
	return 0;
//	return DefWindowProc(hwnd, msg, wParam, lParam);	// let Windows handle it

//	return (spacewar->messageHandler(hwnd, msg, wParam, lParam));
}


//=============================================================================
// ウィンドウクラスの登録とウィンドウ作成
// Returns: false on error
//=============================================================================
bool CreateMainWindow(HWND& hwnd, HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEX wcx;
	
	// Fill in the window class structure with parameters 
	// that describe the main window. 
	wcx.cbSize = sizeof(wcx);				// size of structure 
	wcx.style = CS_HREDRAW | CS_VREDRAW;	// redraw if size changes 
	wcx.lpfnWndProc = WinProc;				// points to window procedure 
	wcx.cbClsExtra = 0;						// no extra class memory 
	wcx.cbWndExtra = 0;						// no extra window memory 
	wcx.hInstance = hInstance;				// handle to instance 
	wcx.hIcon = NULL;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);	// predefined arrow 
//    wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);    // black background 
	wcx.hbrBackground = NULL;    // black background 
	wcx.lpszMenuName = TEXT("MYMENU");		// name of menu resource 
	wcx.lpszClassName = CLASS_NAME;			// name of window class 
	wcx.hIconSm = NULL;						// small class icon 

	// ウィンドウクラスの登録
	if (RegisterClassEx(&wcx) == 0)			// if error
		return false;
	
	// ウィンドウスタイル　全画面表示 or ウィンドウ表示
	DWORD style;
	if (FULLSCREEN)
	{
		style = WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP;
	}
	else
	{
		style = WS_OVERLAPPEDWINDOW;
	}

	// ウィンドウ作成
	hwnd = CreateWindow(
		CLASS_NAME,					// name of the window class
		WIN_TITLE,					// title bar text
		style,						// window style
		CW_USEDEFAULT,				// default horizontal position of window
		CW_USEDEFAULT,				// default vertical position of window
		WIN_WIDTH,					// width of window
		WIN_HEIGHT,					// height of the window
		(HWND)NULL,					// no parent window
		(HMENU)NULL,				// no menu
		hInstance,					// handle to application instance
		(LPVOID)NULL);				// no window parameters

	// if there was an error creating the window
	if (!hwnd)
	{
		return false;
	}

	if (!FULLSCREEN)				// ウィンドウ表示？
	{
		// Adjust window size so client area is WIN_WIDTH x WIN_HEIGHT
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);	// get size of client area of window
		MoveWindow(	hwnd,
					0,						// Left
					0,						// Top
					WIN_WIDTH + (WIN_WIDTH - clientRect.right),		// Right
					WIN_HEIGHT + (WIN_HEIGHT - clientRect.bottom),	// Bottom
					TRUE
					);						// Repaint the window
	}

	// ウィンドウを表示する
	ShowWindow(hwnd, nCmdShow);

	return true;
}


//=============================================================================
// 二重起動のチェック
//=============================================================================
bool AnotherInstance()
{
	HANDLE ourMutex;

	// Attempt to create a mutex using our unique string
	ourMutex = CreateMutex(	NULL,
							true,
							"Use_a_different_string_here_for_each_program_48161-XYZZY"
							);

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return true;            // another instance was found
	}

	return false;               // we are the only instance
}

