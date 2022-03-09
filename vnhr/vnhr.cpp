// vnhr.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "vnhr.h"
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

list<CAPSTRUCT*> pstCapList;
//capstruct under process
list<CAPSTRUCT*> pstCapProcessList;
//list<HBITMAP> hBMPList;
//list<HDC> hDCList;
//list<UINT_PTR> hIDTimerList;


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






void CALLBACK DisplayWndTimerProc(HWND hWnd, UINT message, UINT_PTR iTimerID, DWORD dwTime)
{
    RECT stRect;
    HDC hDCCompat;
    HBITMAP hBitmapCompat;
    CAPSTRUCT stCap;
    WCHAR buffer[128];
    KillTimer(NULL, iTimerID);
    hDCTarget = GetDC(hWndTarget);
    hDCCompat = CreateCompatibleDC(hDCTarget);
    GetClientRect(hWndTarget, &stRect);
    hBitmapCompat = CreateCompatibleBitmap(hDCTarget, stRect.right - stRect.left, stRect.bottom - stRect.top);
    SelectObject(hDCCompat, hBitmapCompat);
    BitBlt(hDCCompat, 0, 0, stRect.right, stRect.bottom, hDCTarget, 0, 0, SRCCOPY);

    ReleaseDC(hWndTarget, hDCTarget);
    stCap.hBitmap = hBitmapCompat;
    stCap.hDC = hDCCompat;
    AlterstCapList(&stCap, HRTASK_APPEND);
    //ViewhDCListLen();
    ReleaseSemaphore(hSemaphoreThread, 1, NULL);
    //ViewhDCListLen();
    //MessageBox(hWnd, L"released!", NULL, MB_OK);
}

LRESULT CALLBACK DisplayWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static WCHAR buffer[128];
    RECT stRect;
    HBITMAP hBMPCompat;
    CAPSTRUCT* pstCap;
    int x, y, i;
    double _x, _y;
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 分析菜单选择:
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 在此处添加使用 hdc 的任何绘图代码...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL:
    case WM_CHAR:
    case WM_KEYDOWN:
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
        // create timers which won't replace each other
        //SetTimer(hWnd, ID_TIMERDISPLAYDELAY, uDisplayDelay, DisplayWndTimerProc);
        //hIDTimerList.push_back(SetTimer(NULL, 0, uDisplayDelay, DisplayWndTimerProc));
        SetTimer(NULL, 0, uDisplayDelay, DisplayWndTimerProc);
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        //wsprintf(buffer, L"%d, %d", x, y);
        ////MessageBox(hWnd, buffer, NULL, MB_OK);
        //SetWindowText(hLabelTest, buffer);
        if (!GetClientRect(hWndTarget, &stRect))
            SendMessage(hWndDisplay, WM_DESTROY, NULL, NULL);
        _x = (double)x * (double)(stRect.right - stRect.left);
        _y = (double)y * (double)(stRect.bottom - stRect.top);
        GetClientRect(hWnd, &stRect);
        x = (WORD)(_x / (double)(stRect.right - stRect.left));
        y = (WORD)(_y / (double)(stRect.bottom - stRect.top));
        error = PostMessage(hWndTarget, message, wParam, MAKELPARAM(x, y));
        //error = GetLastError();
        //wsprintf(buffer, L"%d", error);
        //SetWindowText(hLabelTest3, buffer);
        break;
    case WM_CREATE:
        bThreadExit = false;
        hSemaphoreThread = CreateSemaphore(NULL, 0, ustCapCapacity, NULL);
        hThread = CreateThread(NULL, 0, RunHR, NULL, 0, &dwThreadId);
        hMutexAlter = CreateMutex(NULL, FALSE, NULL); 
        hMutexCreateFile = CreateMutex(NULL, FALSE, NULL);
        //hLabelTest = CreateWindow(TEXT("STATIC"), NULL, WS_VISIBLE | WS_CHILD, cxChar, cyChar, 20 * cxChar, cyChar * 7 / 4, hWnd, (HMENU)ID_LABELTEST, hInst, NULL);
        //hLabelTest2 = CreateWindow(TEXT("STATIC"), NULL, WS_VISIBLE | WS_CHILD, cxChar, cyChar * 2, 20 * cxChar, cyChar * 7 / 4, hWnd, (HMENU)ID_LABELTEST, hInst, NULL);
        //hLabelTest3 = CreateWindow(TEXT("STATIC"), NULL, WS_VISIBLE | WS_CHILD, cxChar, cyChar * 3, 20 * cxChar, cyChar * 7 / 4, hWnd, (HMENU)ID_LABELTEST, hInst, NULL);
        break;
    case WM_DESTROY:
        TerminateThread(hThread, 0);
        for (auto it = pstCapProcessList.cbegin(); it != pstCapProcessList.cend(); ++it)
        {
            pstCap = *it;
            DeleteObject(pstCap->hBitmap);
            DeleteDC(pstCap->hDC);
        }
        for (auto it = pstCapList.cbegin(); it != pstCapList.cend(); ++it)
        {
            pstCap = *it;
            DeleteObject(pstCap->hBitmap);
            DeleteDC(pstCap->hDC);
        }
        AlterstCapList(NULL, HRTASK_CLEAR_ALL);
        //// wait for running threads to finish
        //SwitchToThread();
        CloseHandle(hThread);
        CloseHandle(hMutexAlter);
        CloseHandle(hMutexCreateFile);
        CloseHandle(hSemaphoreThread);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
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
