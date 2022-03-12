#include "vnhrwindow.h"
#include <exception>

const WCHAR VnhrWindow::szWndClassName_[32] = TEXT("VnhrWindow");

bool VnhrWindow::Init(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam)
{
    WCHAR szBuffer[128];
	hInstance_ = hInstance;
	hWndParent_ = hWndParent;
    hWnd_ = CreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, (LPVOID)this);
    wsprintf(szBuffer, L"%x", hWnd_);
    MessageBox(NULL, szBuffer, NULL, MB_OK);
    ShowWindow(hWnd_, SW_SHOWDEFAULT);
    UpdateWindow(hWnd_);
    return true;
}

bool VnhrWindow::Destruction()
{
    DestroyWindow(hWnd_);
    return true;
}

LRESULT VnhrWindow::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hwnd, message, wParam, lParam);
}

ATOM VnhrWindow::RegisterWndClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = DefWindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VNHR));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_VNHR);
    wcex.lpszClassName = VnhrWindow::szWndClassName_;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

LRESULT CALLBACK VnhrWindow::NativeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WCHAR szBuffer[128];
    static bool first = true;
    static bool first_display = true;
    static HWND main_hwnd = NULL;
    static HWND display_hwnd = NULL;
    if (!hWnd)
        return FALSE;
    if (message == WM_NCCREATE)
    {
        //wsprintf(szBuffer, L"WM_NCCREATE %x", hWnd);
        //MessageBox(NULL, szBuffer, NULL, MB_OK);
        // get the ptr of instantiated object and stock it into GWLP_USERDATA index in order to retrieve afterward
        VnhrWindow* pM30ide = (VnhrWindow*)((reinterpret_cast<LPCREATESTRUCT>(lParam))->lpCreateParams);
        pM30ide->hWnd_ = hWnd;
        //if (hWnd != NULL)
        //{
        //    if (!first && hWnd != main_hwnd)
        //    {
        //        wsprintf(szBuffer, L"create display %x", hWnd);
        //        MessageBox(NULL, szBuffer, NULL, MB_OK);
        //        display_hwnd = hWnd;
        //    }
        //    else
        //    {
        //        main_hwnd = hWnd;
        //        wsprintf(szBuffer, L"main_hwnd first %x", hWnd);
        //        MessageBox(NULL, szBuffer, NULL, MB_OK);
        //        first = false;
        //    } 
        //}
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pM30ide));

        return TRUE;
    }
    //wsprintf(szBuffer, L"hWnd %x", hWnd);
    //MessageBox(hWnd, szBuffer, NULL, MB_OK);
    VnhrWindow* obj = GetObjectforWnd(hWnd);
    //wsprintf(szBuffer, L"obj %x", &obj);
    //MessageBox(hWnd, szBuffer, NULL, MB_OK);
    if (obj)
    {
        //if (hWnd != display_hwnd && hWnd != main_hwnd)
        //{
        //    wsprintf(szBuffer, L"message %x", hWnd);
        //    MessageBox(NULL, szBuffer, NULL, MB_OK);

        //}

        return obj->WndProc(hWnd, message, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}