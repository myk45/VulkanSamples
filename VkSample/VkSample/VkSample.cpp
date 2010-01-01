// VkSample.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "VkSample.h"

#include <vulkan/vulkan.h>
#include "vkutils.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND windowHandle;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WindowProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);


extern void vkStuff();

//------------------------------------------------------------------------------

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CLOSE: {
        PostQuitMessage(0);
        break;
    }
    default: {
        break;
    }
    }

    // a pass-through for now. We will return to this callback
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    WNDCLASSEX windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = (LPCWSTR)"VulkanWindowClass";
    RegisterClassEx(&windowClass);
    hInst = hInstance;

    windowHandle = CreateWindowEx(NULL, (LPCWSTR)"VulkanWindowClass", (LPCWSTR)"Core",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        100,
        100,
        800,    // some random values for now. 
        600,    // we will come back to these soon.
        NULL,
        NULL,
        hInstance,
        NULL);

    vkStuff();

    MSG msg;
    bool done = false;
    while (!done) {
        PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE);
        if (msg.message == WM_QUIT) {
            done = true;
        }
        else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        RedrawWindow(windowHandle, NULL, NULL, RDW_INTERNALPAINT);
    }

    return msg.wParam;
}