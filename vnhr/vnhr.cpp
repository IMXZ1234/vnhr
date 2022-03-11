// vnhr.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "vnhr.h"
#include "vnhrdisplaywindow.h"
#include "vnhrmainwindow.h"
#include <list>
#include <vector>
//#include "opencv2/opencv.hpp"

#define MAX_LOADSTRING 100
using namespace std;

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWndClassMain[MAX_LOADSTRING];
WCHAR szWndClassDisplay[MAX_LOADSTRING];
HWND hWndTarget;
HWND hWndDisplay;
HWND hEditWndTarget;
HWND hButtonStart;
HWND hLabelTest;
HWND hLabelTest2;
HWND hLabelTest3;
HDC hDCDisplay;
HDC hDCTarget;
HANDLE hThread;
DWORD dwThreadId;
BOOL bThreadExit;
CRITICAL_SECTION stCS;
HANDLE hMutexAlter;
HANDLE hMutexCreateFile;
HANDLE hSemaphoreThread;
DWORD error;
WCHAR buffer[128];
UINT uDisplayDelay = 400;
UINT ustCapCapacity = 1;
HDC hDCMem;
UINT uHRScale = 4;
UINT uScreenX, uScreenY;

WCHAR szCurrentDir[128];
WCHAR szHRExePath[] = L"C:\\Users\\asus\\Downloads\\Compressed\\realesrgan-ncnn-vulkan-20211212-windows\\realesrgan-ncnn-vulkan.exe";
WCHAR szffmpegExePath[] = L"C:\\Users\\asus\\coding\\vsc++\\ffmpeg-4.4.1-full_build-shared\\bin\\ffmpeg.exe";
WCHAR szHRExeDir[] = L"C:\\Users\\asus\\Downloads\\Compressed\\realesrgan-ncnn-vulkan-20211212-windows";
WCHAR szModelrealesrgan_x4plus_anime[] = L"realesrgan-x4plus-anime";
WCHAR szCmdLineBase[] = L"%s -i %s -o %s -s %d -n %s";
WCHAR szffmpegBase[] = L"%s -i %s %s";
WCHAR szffmpegResizeBase[] = L"%s -i %s -s %dx%d %s";
//WCHAR szffmpegBase[] = L"C:\Users\asus\coding\vsc++\ffmpeg-4.4.1-full_build-shared\bin\ffmpeg -i %s %s";



DWORD baseUnit = GetDialogBaseUnits();
int cxChar = LOWORD(baseUnit);
int cyChar = HIWORD(baseUnit);



// 此代码模块中包含的函数的前向声明:
ATOM                VNHRMainClass(HINSTANCE hInstance);
ATOM                VNHRDisplayClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    DisplayWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                SaveBitmapToFile(HDC hDC, HBITMAP hBitmap, const WCHAR* path);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_VNHRMAIN, szWndClassMain, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_VNHRDISPLAY, szWndClassDisplay, MAX_LOADSTRING);
    VNHRMainClass(hInstance);
    VNHRDisplayClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VNHR));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM VNHRMainClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VNHR));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_VNHR);
    wcex.lpszClassName = szWndClassMain;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

ATOM VNHRDisplayClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = DisplayWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VNHR));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWndClassDisplay;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWndClassMain, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 300, 200, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
