#pragma once
#include "framework.h"
#include "util.h"
#include "vnhrmainwindow.h"

#define ID_EDITWNDTARTGET	1
#define ID_BUTTONSTART		2
#define ID_LABELTEST		3

class VnhrMainWindow
{
public:
	VnhrMainWindow();
	~VnhrMainWindow();
	VnhrMainWindow GetInstance();
private:
	static VnhrMainWindow instance_;
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HWND hEditWndTarget;

};

