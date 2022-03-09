#pragma once
#include "vnhr.h"
#include "hrprocessor.h"
#include "vnhrwindow.h"
#include <list>
#include <map>

#define ID_TIMERDISPLAYDELAY 1

class VnhrDisplayWindow :
	public VnhrWindow
{
public:

private:
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
	HANDLE hSemaphoreThread;
	DWORD error;
	WCHAR buffer[128];
	UINT uDisplayDelay = 400;
	UINT ustCapCapacity = 1;
	HDC hDCMem;
	UINT uHRScale = 4;
	UINT uScreenX, uScreenY;


	DWORD baseUnit = GetDialogBaseUnits();
	int cxChar = LOWORD(baseUnit);
	int cyChar = HIWORD(baseUnit);

	static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

