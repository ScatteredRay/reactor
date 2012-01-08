// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#include <assert.h>

#include "view_decl.h"
#include "gl_all.h"

HINSTANCE WinSys_hInstance;
ViewInfo* WinSys_View;

int GetKeyCodeFromAscii(char key)
{
    // Windows Key Codes are the same for Ascii numbers and upper case.

    if((key >= '0' && key <= '9') ||
       (key >= 'A' && key <= 'Z'))
        return (int)key;
    if(key >= 'a' && key <= 'z')
        return (int)(key - 'a' + 'A');

    return -1;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int InitWindowAndLoop(int argc, char** argv)
{
    char* ClassName = "BOOTEDWIN";
    WNDCLASSEX ClassStruct;
    memset(&ClassStruct, 0, sizeof(ClassStruct));
    ClassStruct.cbSize = sizeof(ClassStruct);
    ClassStruct.style = CS_OWNDC;
    ClassStruct.lpfnWndProc = WndProc;
    ClassStruct.hInstance = WinSys_hInstance;
    ClassStruct.lpszClassName = ClassName;
    RegisterClassEx(&ClassStruct);
    HWND hWnd = CreateWindow(ClassName,
                             "BootEd Window.",
                             WS_TILEDWINDOW,
                             CW_USEDEFAULT,
                             CW_USEDEFAULT,
                             1024,
                             768,
                             NULL,
                             NULL,
                             WinSys_hInstance,
                             NULL);

    ShowWindow(hWnd, SW_SHOW);

    HDC dc = GetDC(hWnd);

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;
 
	int pixel_format = ChoosePixelFormat(dc, &pfd);
    if(pixel_format == 0)
        return 1;
 
	if(!SetPixelFormat(dc, pixel_format, &pfd))
        return 1;

    HGLRC glRc = wglCreateContext(dc);
    wglMakeCurrent(dc, glRc);

    void InitGLExt();
    InitGLExt();

    WinSys_View = InitView();
    ResizeView(WinSys_View, 0, 0);

    // This message loop is meant to consume all system resources, needs
    // to be changed for apps that are meant to idle, I.E. editors.
    bool continue_loop = true;
    while(continue_loop)
    {
        MSG msg;
        while(PeekMessage(&msg, NULL, 0, 0, 0))
        {
            if(!GetMessage(&msg, NULL, 0, 0))
                continue_loop = false;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        UpdateView(WinSys_View);
        SwapBuffers(dc);
    }

    FinishView(WinSys_View);
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(glRc);

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    MouseStatus status;
    MouseButton button = No_Button;
    switch(msg)
    {
    case WM_SIZE:
        ResizeView(WinSys_View, LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
        // (lParam bit 30) == 1 means the key was already down.
        if((lParam & (1 << 30)) == 0)
            InputReceiveKey (GetInputHandler(WinSys_View),
                            wParam,
                            Key_Down);
        break;
    case WM_KEYUP:
        InputReceiveKey(GetInputHandler(WinSys_View),
                        wParam,
                        Key_Up);
        break;
    case WM_MOUSEMOVE:
        status = Mouse_Move;
        goto mouseevent;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
        status = Mouse_Down;
        goto mouseevent;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
        status = Mouse_Up;
        goto mouseevent;
mouseevent:
        if(wParam & MK_LBUTTON)
            button = (MouseButton)(button | Left_Button);
        else if(wParam & MK_RBUTTON)
            button = (MouseButton)(button | Right_Button);
        else if(wParam & MK_MBUTTON)
            button = (MouseButton)(button | Mid_Button);

        InputMouseEvent(GetInputHandler(WinSys_View),
                        LOWORD(lParam),
                        HIWORD(lParam),
                        button,
                        status);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

void InitGLExt()
{
#define GL_DECL(fun, decl) fun = (decl)wglGetProcAddress(#fun);
#include "glext_loader.inc"
#undef GL_DECL
}

#define GL_DECL(fun, decl) decl fun;
#include "glext_loader.inc"
#undef GL_DECL
