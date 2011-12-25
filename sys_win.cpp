// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#include <assert.h>

#include "view_decl.h"
#include "gl_all.h"

HINSTANCE WinSys_hInstance;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int InitWindowAndLoop(int argc, char** argv)
{
    char* ClassName = "BOOTEDWIN";
    WNDCLASSEX ClassStruct;
    memset(&ClassStruct, 0, sizeof(ClassStruct));
    ClassStruct.cbSize = sizeof(ClassStruct);
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
    }

    return 0;
    void InitGLExt();
    InitGLExt();

    ViewInfo* view = InitView();
    ResizeView(view, 0, 0);
    UpdateView(view);
    FinishView(view);
    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
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
