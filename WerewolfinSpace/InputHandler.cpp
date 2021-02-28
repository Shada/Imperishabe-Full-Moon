#include "InputHandler.h"


InputHandler::InputHandler( HINSTANCE hInstance, HWND* hWnd )
{
    din            = 0;
    dinKeyboard = 0;
    dinMouse    = 0;
    initDInput( hInstance, hWnd );
}
HRESULT InputHandler::initDInput( HINSTANCE hInstance, HWND* hWnd )
{
    HRESULT result;
    result = DirectInput8Create(hInstance,
                                DIRECTINPUT_VERSION,
                                IID_IDirectInput8,
                                (void**)&din,
                                NULL);
    
    if(FAILED( din->CreateDevice(    GUID_SysKeyboard,
                                    &dinKeyboard,
                                    NULL )))
        return result;

    result = din->CreateDevice(    GUID_SysMouse,
                                &dinMouse,
                                NULL );

    if(FAILED(result = dinKeyboard->SetDataFormat( &c_dfDIKeyboard )))
        return result;

    if(FAILED(result = dinMouse->SetDataFormat( &c_dfDIMouse )))
        return result;

    if(FAILED(result = dinKeyboard->SetCooperativeLevel( *hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND )))
        return result;

    if(FAILED(result = dinMouse->SetCooperativeLevel( *hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE )))
        return result;

    if(FAILED(result = dinKeyboard->Acquire()))
        return result;

    if(FAILED(result = dinMouse->Acquire()))
        return result;
    
    return result;
}
void InputHandler::Update()
{
    for( int i = 0; i < 256; i++)
        prevKeyState[i] = keyState[i];

    prevMouseState = mouseState;
    
    HRESULT result;
    result = dinKeyboard->GetDeviceState( 256, (LPVOID)&keyState );

    if(FAILED(result))
        if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
            dinKeyboard->Acquire();

    result = dinMouse->GetDeviceState( sizeof(DIMOUSESTATE), (LPVOID)&mouseState );
    
    if(FAILED(result))
        if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
            dinMouse->Acquire();
}
InputHandler::~InputHandler()
{

}
