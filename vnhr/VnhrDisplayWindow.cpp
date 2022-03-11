#include "VnhrDisplayWindow.h"

const WCHAR VnhrDisplayWindow::szWndClassName_[32] = TEXT("VnhrDisplayWindow");

void CALLBACK VnhrDisplayWindow::DisplayWndTimerProc(HWND hWnd, UINT message, UINT_PTR idTimer, DWORD dwTime)
{
    VnhrDisplayWindow* obj = (VnhrDisplayWindow*)VnhrDisplayWindow::GetObjectforWnd(hWnd);
    if (idTimer != obj->auto_timerid_)
        KillTimer(hWnd, idTimer);
    HRPROCESSTASK process_task;
    RECT stRectFrom, stRectTo;
    BITMAPINFOHEADER* pbmih = obj->img_capture_->CaptureAsBitmap(hWnd);
    process_task.hWnd = hWnd;
    process_task.pbmih = pbmih;
    GetClientRect(hWnd, &stRectTo);
    GetClientRect(obj->hWndTarget_, &stRectFrom);
    memcpy_s(&(process_task.stRectFrom), sizeof(RECT), &stRectFrom, sizeof(RECT));
    memcpy_s(&(process_task.stRectTo), sizeof(RECT), &stRectTo, sizeof(RECT));
    obj->processor_->ProcessHR(&process_task);
}

ATOM VnhrDisplayWindow::RegisterWndClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = VnhrDisplayWindow::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VNHR));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_VNHR);
    wcex.lpszClassName = VnhrDisplayWindow::szWndClassName_;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

bool VnhrDisplayWindow::Init(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HRPROCESSCONFIG process_config;
    hr_cache_ = new HRCache();
    hr_model_ = new RealesrganHRModel();
    img_capture_ = new GDIImgCapture();
    processor_ = HRProcessor::GetInstance();
    auto_mode_ = false;
    auto_timerid_ = 0;
    uAutoInterval = 1000;
    uDisplayDelay = 400;
    hWndTarget_ = NULL;
    pbmih_ = NULL;
    if (VnhrWindow::Init(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam))
        return false;
    process_config.cache = hr_cache_;
    process_config.model = hr_model_;
    processor_->Register(hWnd_, &process_config);
    return true;
}

bool VnhrDisplayWindow::Destruction()
{
    processor_->Unregister(hWnd_);
    delete hr_cache_;
    delete hr_model_;
    delete img_capture_;
    return VnhrWindow::Destruction();
}

inline bool VnhrDisplayWindow::set_target_window(HWND hWndTarget)
{
    hWndTarget_ = hWndTarget;
    return true;
}

inline HWND VnhrDisplayWindow::GetTargetWindow()
{
    return hWndTarget_;
}

inline bool VnhrDisplayWindow::SetImgCapture(ImgCapture* img_capture)
{
    img_capture_ = img_capture;
    return true;
}

inline ImgCapture* VnhrDisplayWindow::GetImgCapture()
{
    return img_capture_;
}

inline bool VnhrDisplayWindow::SetHRCache(HRCache* hr_cache)
{
    hr_cache_ = hr_cache;
    return true;
}

inline HRCache* VnhrDisplayWindow::GetHRCache()
{
    return hr_cache_;
}

inline bool VnhrDisplayWindow::SetHRModel(HRModel* hr_model)
{
    hr_model_ = hr_model;
    return true;
}

inline HRModel* VnhrDisplayWindow::get_hr_model()
{
    return hr_model_;
}

inline HRProcessor* VnhrDisplayWindow::get_processor()
{
    return processor_;
}

LRESULT CALLBACK VnhrDisplayWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static WCHAR szBuffer[128];
    RECT stRect;
    int x, y, i;
    double _x, _y;
    VnhrDisplayWindow* obj = (VnhrDisplayWindow*)GetObjectforWnd(hWnd);
    UINT idTimer;
    switch (message)
    {
    case VNHRM_FREE_BMP:
        free((LPVOID)wParam);
        break;
    case VNHRM_HRFINISHED:
        obj->pbmih_ = (BITMAPINFOHEADER*)wParam;
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // ·ÖÎö²Ëµ¥Ñ¡Ôñ:
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        case IDC_AUTO:
            MessageBox(hWnd, L"Install", NULL, MB_OK);
            obj->auto_timerid_ = IDAllocator::GetIDAllocatorFor(IDALLOCATOR_TIMER)->AllocateID();
            if (obj->auto_timerid_ == -1)
                break;
            idTimer = SetTimer(hWnd, obj->auto_timerid_, obj->uAutoInterval, VnhrDisplayWindow::DisplayWndTimerProc);
            wsprintf(szBuffer, L"timer SET %x", idTimer);
            MessageBox(NULL, szBuffer, NULL, MB_OK);
            //hDllhk = LoadLibrary(L"hk.dll");
            break;
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        //GetClientRect(hWndDisplay, &stRect);
        //StretchDIBits(hDCDisplay, 0, 0, stRect.right - stRect.left, stRect.bottom - stRect.top, 0, 0, w, h, pBits, pbmi, DIB_RGB_COLORS, SRCCOPY);
        if (obj->pbmih_)
            SetDIBitsToDevice(hdc, 0, 0, obj->pbmih_->biWidth, obj->pbmih_->biWidth, 0, 0, 0, obj->pbmih_->biHeight, 
                obj->pbmih_ + sizeof(BITMAPINFOHEADER), (BITMAPINFO*)obj->pbmih_, DIB_RGB_COLORS);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL:
    case WM_CHAR:
    case WM_KEYDOWN:
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
        // create timers which won't replace each other
        //SetTimer(hWnd, ID_TIMERDISPLAYDELAY, uDisplayDelay, DisplayWndTimerProc);
        //hIDTimerList.push_back(SetTimer(NULL, 0, uDisplayDelay, DisplayWndTimerProc));
        SetTimer(NULL, 0, obj->uDisplayDelay, DisplayWndTimerProc);
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        //wsprintf(buffer, L"%d, %d", x, y);
        ////MessageBox(hWnd, buffer, NULL, MB_OK);
        //SetWindowText(hLabelTest, buffer);
        if (!GetClientRect(obj->hWndTarget_, &stRect))
            SendMessage(hWnd, WM_DESTROY, NULL, NULL);
        _x = (double)x * (double)(stRect.right - stRect.left);
        _y = (double)y * (double)(stRect.bottom - stRect.top);
        GetClientRect(hWnd, &stRect);
        x = (WORD)(_x / (double)(stRect.right - stRect.left));
        y = (WORD)(_y / (double)(stRect.bottom - stRect.top));
        PostMessage(obj->hWndTarget_, message, wParam, MAKELPARAM(x, y));
        //error = GetLastError();
        //wsprintf(buffer, L"%d", error);
        //SetWindowText(hLabelTest3, buffer);
        break;
    case WM_CREATE:
        break;
    case WM_DESTROY:
        obj->Destruction();
        delete obj;
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}