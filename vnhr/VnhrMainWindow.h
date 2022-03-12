#pragma once
#include "vnhr.h"
#include "vnhrwindow.h"
#include "vnhrdisplaywindow.h"
#include "util.h"

#define ID_EDITHWNDTARTGET	1
#define ID_BUTTONSTART		2
#define ID_LABELTEST		3

#define ID_STATICSEARCHTARGET   4

class VnhrMainWindow :
	public VnhrWindow
{
public:
	static ATOM RegisterWndClass(HINSTANCE hInstance);
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

	static const WCHAR szWndClassName_[32];
private:
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    static INT_PTR About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	HWND hEdithWndTarget;
	HWND hButtonStart;
    HWND hStaticSearchTarget;
	bool bSearchingTarget;

};

