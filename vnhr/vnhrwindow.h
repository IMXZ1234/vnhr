#pragma once
#include "framework.h"
#include <map>

class VnhrWindow
{
public:
    VnhrWindow() = default;
    ~VnhrWindow() = default;
	// creates the window and insert a pair of hWnd and this VnhrWindow instance's pointer into window_object_map
	// has the same param as CreateWindowEx()
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
	
	inline static VnhrWindow* GetObjectforWnd(HWND hWnd);
protected:
    virtual ATOM RegisterWndClass();
    static bool window_class_registered;

	static std::map<HWND, VnhrWindow*> window_object_map_;
	HINSTANCE hInstance_;
	HWND hWndParent_;
	HWND hWnd_;
};

