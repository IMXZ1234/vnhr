#pragma once
#include "vnhr.h"
#include "hrprocessor.h"
#include "vnhrwindow.h"
#include "idallocator.h"
#include "gdiimgcapture.h"
#include "realesrganhrmodel.h"
#include "hrcache.h"
#include <list>
#include <map>

#define ID_TIMERDISPLAYDELAY	1
#define IDC_AUTO				1

class VnhrDisplayWindow :
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
	static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static IDAllocator idallocator_;

	UINT uDisplayDelay = 400;
	ImgCapture* img_capture_;
    HRModel* hr_model_;
	HRCache* cache_;
	
	HRProcessor* processor_ = HRProcessor::GetInstance();
    bool auto_mode_;
};

