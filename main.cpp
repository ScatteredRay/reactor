// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
int InitWindowAndLoop(int argc, char** argv);

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
// argc and __argv in windows.
#include <stdlib.h>
extern HINSTANCE WinSys_hInstance;
#endif

#ifdef _WIN32
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char** argv)
#endif
{
#ifdef _WIN32
    WinSys_hInstance = hInstance;
    int argc = __argc;
    char** argv = __argv;
#endif
    return InitWindowAndLoop(argc, argv);
}
