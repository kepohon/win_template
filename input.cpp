// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// input.cpp v1.0

#include "input.h"

//=============================================================================
// default constructor
// ・キー状態の配列を初期化
// ・各種変数の初期化
//=============================================================================
Input::Input()
{
    // clear array
	for (size_t i = 0; i < inputNS::KEYS_ARRAY_LEN; i++)
	{
		keysDown[i] = false;
		keysPressed[i] = false;
	}
//    for (size_t i = 0; i < inputNS::KEYS_ARRAY_LEN; i++)
//        keysDown[i] = false;
    // clear key pressed array
//    for (size_t i = 0; i < inputNS::KEYS_ARRAY_LEN; i++)
//        keysPressed[i] = false;
    newLine = true;                     // start new line
    textIn = "";                        // clear textIn
    charIn = 0;                         // clear charIn

    // mouse data
    mouseX = 0;                         // screen X
    mouseY = 0;                         // screen Y
    mouseRawX = 0;                      // high-definition X
    mouseRawY = 0;                      // high-definition Y
    mouseLButton = false;               // true if left mouse button is down
    mouseMButton = false;               // true if middle mouse button is down
    mouseRButton = false;               // true if right mouse button is down
    mouseX1Button = false;              // true if X1 mouse button is down
    mouseX2Button = false;              // true if X2 mouse button is down

    for(int i=0; i<MAX_CONTROLLERS; i++)
    {
        controllers[i].vibrateTimeLeft = 0;
        controllers[i].vibrateTimeRight = 0;
    }
}

//=============================================================================
// destructor
// マウスのキャプチャーが設定されていれば、キャプチャーを解放する
//=============================================================================
Input::~Input()
{
    if(mouseCaptured)
        ReleaseCapture();               // release mouse
}

//=============================================================================
// Initialize mouse and controller input
// Set capture=true to capture mouse
// Throws GameError
// マウスとコントローラーの入力を初期化
// ・高精度マウスを登録する
// ・マウスをキャプチャする場合、capture=trueを設定
// ・コントローラーの状態をクリアする
// GameErrorをスロー
//=============================================================================
void Input::initialize(HWND hwnd, bool capture)
{
    try{
        mouseCaptured = capture;

        // 高精度マウスを登録
        Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC; 
        Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE; 
        Rid[0].dwFlags = RIDEV_INPUTSINK;   
        Rid[0].hwndTarget = hwnd;
        RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

        if(mouseCaptured)
            SetCapture(hwnd);           // capture mouse

        // コントローラーの状態をクリア
        ZeroMemory( controllers, sizeof(ControllerState) * MAX_CONTROLLERS );

        checkControllers();             // check for connected controllers
    }
    catch(...)
    {
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing input system"));
    }
}

//=============================================================================
// Set true in the keysDown and keysPessed array for this key
// Pre: wParam contains the virtual key code (0--255)
// 入力されたキーをkeysDown配列とkeysPressed配列にセットする
// 実行前：ｗParamに、仮想キーコード（0〜255）が格納されている
//=============================================================================
void Input::keyDown(WPARAM wParam)
{
    // キーコードがバッファ範囲内にあることを確認
    if (wParam < inputNS::KEYS_ARRAY_LEN)
    {
        keysDown[wParam] = true;    // update keysDown array
        // key has been "pressed, erased by clear()
        keysPressed[wParam] = true; // update keysPressed array
    }
}

//=============================================================================
// Set false in the keysDown array for this key
// Pre: wParam contains the virtual key code (0--255)
// 指定のキーが離されたことをkeysDown配列にセットする
//=============================================================================
void Input::keyUp(WPARAM wParam)
{
    // make sure key code is within buffer range
    if (wParam < inputNS::KEYS_ARRAY_LEN)
        // update state table
        keysDown[wParam] = false;
}

//=============================================================================
// Save the char just entered in textIn string
// Pre: wParam contains the char
// 入力された文字をtextIn文字列に保存する
//=============================================================================
void Input::keyIn(WPARAM wParam)
{
    if (newLine)                            // if start of new line
    {
        textIn.clear();
        newLine = false;
    }

    if (wParam == '\b')                     // if backspace
    {
        if(textIn.length() > 0)             // if characters exist
            textIn.erase(textIn.size()-1);  // erase last character entered
    }
    else
    {
        textIn += wParam;                   // add character to textIn
        charIn = wParam;                    // save last char entered
    }

    if ((char)wParam == '\r')               // if return    
        newLine = true;                     // start new line

	if ((char)wParam == ESC_KEY)			// if ESC key
		PostQuitMessage(0);					// window destroy

}

//=============================================================================
// Returns true if the specified VIRTUAL KEY is down, otherwise false.
// 指定された仮想キーが押されている場合はtrue、それ以外の場合はfalseを返す
//=============================================================================
bool Input::isKeyDown(UCHAR vkey) const
{
    if (vkey < inputNS::KEYS_ARRAY_LEN)
        return keysDown[vkey];
    else
        return false;
}

//=============================================================================
// Return true if the specified VIRTUAL KEY has been pressed in the most recent
// frame. Key presses are erased at the end of each frame.
// 直近のフレームにおいて、指定の仮想キーが押されたことがあるばあい、trueを返す。
// キーの押し下げの状態は、各フレームの終了時に消去される。
//=============================================================================
bool Input::wasKeyPressed(UCHAR vkey) const
{
    if (vkey < inputNS::KEYS_ARRAY_LEN)
        return keysPressed[vkey];
    else
        return false;
}

//=============================================================================
// Return true if any key was pressed in the most recent frame.
// Key presses are erased at the end of each frame.
// 直近のフレームにおいて、何らかのキーが押された場合、trueを返す。
// キーの押し下げの状態は、各フレームの終了時に消去される。
//=============================================================================
bool Input::anyKeyPressed() const
{
    for (size_t i = 0; i < inputNS::KEYS_ARRAY_LEN; i++)
        if(keysPressed[i] == true)
            return true;
    return false;
}

//=============================================================================
// Clear the specified key press
//=============================================================================
void Input::clearKeyPress(UCHAR vkey)
{
    if (vkey < inputNS::KEYS_ARRAY_LEN)
        keysPressed[vkey] = false;
}

//=============================================================================
// Clear specified input buffers
// See input.h for what values
//=============================================================================
void Input::clear(UCHAR what)
{
    if(what & inputNS::KEYS_DOWN)       // if clear keys down
    {
        for (size_t i = 0; i < inputNS::KEYS_ARRAY_LEN; i++)
            keysDown[i] = false;
    }
    if(what & inputNS::KEYS_PRESSED)    // if clear keys pressed
    {
        for (size_t i = 0; i < inputNS::KEYS_ARRAY_LEN; i++)
            keysPressed[i] = false;
    }
    if(what & inputNS::MOUSE)           // if clear mouse
    {
        mouseX = 0;
        mouseY = 0;
        mouseRawX = 0;
        mouseRawY = 0;
    }
    if(what & inputNS::TEXT_IN)
        clearTextIn();
}

//=============================================================================
// Reads mouse screen position into mouseX, mouseY
//=============================================================================
void Input::mouseIn(LPARAM lParam)
{
    mouseX = GET_X_LPARAM(lParam); 
    mouseY = GET_Y_LPARAM(lParam);
}

//=============================================================================
// Reads raw mouse data into mouseRawX, mouseRawY
// This routine is compatible with a high-definition mouse
//=============================================================================
void Input::mouseRawIn(LPARAM lParam)
{
    UINT dwSize = 40;
    static BYTE lpb[40];
    
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, 
                    lpb, &dwSize, sizeof(RAWINPUTHEADER));
    
    RAWINPUT* raw = (RAWINPUT*)lpb;
    
    if (raw->header.dwType == RIM_TYPEMOUSE) 
    {
        mouseRawX = raw->data.mouse.lLastX;
        mouseRawY = raw->data.mouse.lLastY;
    } 
}

//=============================================================================
// Check for connected controllers
//=============================================================================
void Input::checkControllers()
{
    DWORD result;
    for( DWORD i = 0; i <MAX_CONTROLLERS; i++)
    {
        result = XInputGetState(i, &controllers[i].state);
        if(result == ERROR_SUCCESS)
            controllers[i].connected = true;
        else
            controllers[i].connected = false;
    }
}

//=============================================================================
// Read state of connected controllers
//=============================================================================
void Input::readControllers()
{
    DWORD result;
    for( DWORD i = 0; i <MAX_CONTROLLERS; i++)
    {
        if(controllers[i].connected)
        {
            result = XInputGetState(i, &controllers[i].state);
            if(result == ERROR_DEVICE_NOT_CONNECTED)    // if controller disconnected
                controllers[i].connected = false;
        }
    }
}

//=============================================================================
// Vibrate connected controllers
//=============================================================================
void Input::vibrateControllers(float frameTime)
{
    for(int i=0; i < MAX_CONTROLLERS; i++)
    {
        if(controllers[i].connected)
        {
            controllers[i].vibrateTimeLeft -= frameTime;
            if(controllers[i].vibrateTimeLeft < 0)
            {
                controllers[i].vibrateTimeLeft = 0;
                controllers[i].vibration.wLeftMotorSpeed = 0;
            }
            controllers[i].vibrateTimeRight -= frameTime;
            if(controllers[i].vibrateTimeRight < 0)
            {
                controllers[i].vibrateTimeRight = 0;
                controllers[i].vibration.wRightMotorSpeed = 0;
            }
            XInputSetState(i, &controllers[i].vibration);
        }
    }
}
