#pragma once
#include "vnhr.h"
#include <list>

#define ID_EDITWNDTARTGET	1
#define ID_BUTTONSTART		2
#define ID_LABELTEST		3

#define ID_TIMERDISPLAYDELAY 1


WCHAR szHRExePath[] = L"C:\\Users\\asus\\Downloads\\Compressed\\realesrgan-ncnn-vulkan-20211212-windows\\realesrgan-ncnn-vulkan.exe";
WCHAR szffmpegExePath[] = L"C:\\Users\\asus\\coding\\vsc++\\ffmpeg-4.4.1-full_build-shared\\bin\\ffmpeg.exe";
WCHAR szHRExeDir[] = L"C:\\Users\\asus\\Downloads\\Compressed\\realesrgan-ncnn-vulkan-20211212-windows";
WCHAR szModelrealesrgan_x4plus_anime[] = L"realesrgan-x4plus-anime";
WCHAR szCmdLineBase[] = L"%s -i %s -o %s -s %d -n %s";
WCHAR szffmpegBase[] = L"%s -i %s %s";
WCHAR szffmpegResizeBase[] = L"%s -i %s -s %dx%d %s";

class VnhrDisplayWindow
{
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

	std::list<CAPSTRUCT*> pstCapList;
	std::list<CAPSTRUCT*> pstCapProcessList;


	DWORD baseUnit = GetDialogBaseUnits();
	int cxChar = LOWORD(baseUnit);
	int cyChar = HIWORD(baseUnit);

	VnhrDisplayWindow();
	DWORD WINAPI RunHR(PVOID lParam);
};

