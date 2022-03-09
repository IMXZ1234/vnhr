#pragma once
#include "vnhr.h"
#include "vnhrwindow.h"
#include "vnhrdisplaywindow.h"
#include "util.h"

#define ID_EDITWNDTARTGET	1
#define ID_BUTTONSTART		2
#define ID_LABELTEST		3

class VnhrMainWindow :
	public VnhrWindow
{
public:

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HWND hEditWndTarget;
	HWND hButtonStart;

};

