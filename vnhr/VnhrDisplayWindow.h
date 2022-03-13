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

    bool set_target_window(HWND hWndTarget);

    inline HWND get_target_window()
    {
        return hWndTarget_;
    }

    inline bool set_img_capture(ImgCapture* img_capture)
    {
        img_capture_ = img_capture;
        return true;
    }

    inline ImgCapture* get_img_capture()
    {
        return img_capture_;
    }

    inline bool set_hr_cache(HRCache* hr_cache)
    {
        hr_cache_ = hr_cache;
        return true;
    }

    inline HRCache* get_hr_cache()
    {
        return hr_cache_;
    }

    inline bool set_hr_model(HRModel* hr_model)
    {
        hr_model_ = hr_model;
        return true;
    }

    inline HRModel* get_hr_model()
    {
        return hr_model_;
    }

    inline HRProcessor* get_processor()
    {
        return processor_;
    }


    static const WCHAR szWndClassName_[32];
private:
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static void CALLBACK DisplayWndTimerProc(HWND hWnd, UINT message, UINT_PTR idTimer, DWORD dwTime);

	UINT uDisplayDelay;
    bool auto_mode_;
    UINT uAutoInterval;
    int auto_timerid_;

    BITMAPINFOHEADER* pbmih_;
    HBITMAP hBitmap_;
    HDC hDCDisplayCache_;

	ImgCapture* img_capture_;
    HRModel* hr_model_;
	HRCache* hr_cache_;

	HRProcessor* processor_;

    HWND hWndTarget_;
};

