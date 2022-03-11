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
    virtual bool Destruction();

    inline bool set_target_window(HWND hWndTarget);
    inline HWND GetTargetWindow();
    inline bool SetImgCapture(ImgCapture* img_capture);
    inline ImgCapture* GetImgCapture();
    inline bool SetHRCache(HRCache* hr_cache);
    inline HRCache* GetHRCache();
    inline bool SetHRModel(HRModel* hr_model);
    inline HRModel* get_hr_model();
    inline HRProcessor* get_processor();

    static const WCHAR szWndClassName_[32];
private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static void CALLBACK DisplayWndTimerProc(HWND hWnd, UINT message, UINT_PTR idTimer, DWORD dwTime);

	UINT uDisplayDelay;
    bool auto_mode_;
    UINT uAutoInterval;
    int auto_timerid_;

    BITMAPINFOHEADER* pbmih_;

	ImgCapture* img_capture_;
    HRModel* hr_model_;
	HRCache* hr_cache_;

	HRProcessor* processor_;

    HWND hWndTarget_;
};

