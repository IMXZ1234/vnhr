#include "vnhrwindow.h"

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
    hWnd_ = CreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	hInstance_ = hInstance;
	hWndParent_ = hWndParent;
    window_object_map_.insert(std::pair<HWND, VnhrWindow*>(hWnd_, this));
}

bool VnhrWindow::Destruction()
{
    DestroyWindow(hWnd_);
    window_object_map_.erase(hWnd_);
}

inline VnhrWindow* VnhrWindow::GetObjectforWnd(HWND hWnd)
{
	std::map<HWND, VnhrWindow*>::iterator it = window_object_map_.find(hWnd);
	if (it != window_object_map_.end())
		return (*it).second;
	return nullptr;
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