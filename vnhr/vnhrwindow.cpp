#include "vnhrwindow.h"

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

ATOM VnhrWindow::RegisterWndClass()
{
    return 0;
}
