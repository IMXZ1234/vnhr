#pragma once
#include "vnhr.h"
#include <map>

class VnhrWindow
{
public:
    VnhrWindow() = default;
    ~VnhrWindow() = default;
	// creates the window and insert a pair of hWnd and this VnhrWindow instance's pointer into window_object_map
	// has the same param as CreateWindowEx()
	static ATOM RegisterWndClass(HINSTANCE hInstance);
	static VnhrWindow* GetObjectforWnd(HWND hWnd);

    virtual bool Init(
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
         LPVOID lpParam);
    virtual bool Destruction();


    static const WCHAR szWndClassName_[32];
protected:
	static std::map<HWND, VnhrWindow*> window_object_map_;

	HINSTANCE hInstance_;
	HWND hWndParent_;
	HWND hWnd_;
};

