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
    wcex.lpfnWndProc = NativeWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VNHR));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = VnhrDisplayWindow::szWndClassName_;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

bool VnhrDisplayWindow::Init(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HRPROCESSCONFIG process_config;
    bool ret_value;
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
    hBitmap_ = NULL;
    hDCDisplayCache_ = NULL;
    ret_value = VnhrWindow::Init(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    process_config.cache = hr_cache_;
    process_config.model = hr_model_;
    processor_->Register(hWnd_, &process_config);
    return ret_value;
}

bool VnhrDisplayWindow::Destruction()
{
    processor_->Unregister(hWnd_);
    delete hr_cache_;
    delete hr_model_;
    delete img_capture_;
    if (hDCDisplayCache_)
        DeleteObject(hDCDisplayCache_);
    if (hBitmap_)
        DeleteObject(hBitmap_);
    return VnhrWindow::Destruction();
}

bool VnhrDisplayWindow::set_target_window(HWND hWndTarget)
{
    hWndTarget_ = hWndTarget;
    HDC hDCTarget = GetDC(hWndTarget_);
    if (hDCDisplayCache_)
        DeleteObject(hDCDisplayCache_);
    hDCDisplayCache_ = CreateCompatibleDC(hDCTarget);
    ReleaseDC(hWndTarget_, hDCTarget);
    if (!hDCDisplayCache_)
        return false;
    return true;
}

// relay messages(mouse/keyboard input) to hWndTarget with tweaks
static BOOL RelayMessages(HWND hWnd, HWND hWndTarget, UINT message, WPARAM wParam, LPARAM lParam)
{
    int x, y, _x, _y;
    RECT stRect;
    switch (message)
    {
    case WM_CHAR:
    case WM_KEYDOWN:
    case WM_KEYUP:
        return PostMessage(hWndTarget, message, wParam, lParam);
    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL:
    case WM_NCLBUTTONDOWN:
    case WM_NCRBUTTONDOWN:
    case WM_NCLBUTTONUP:
    case WM_NCRBUTTONUP:
        // for NC messages, lParam is x, y pos in screen coordinate

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
    case WM_LBUTTONDBLCLK:
    case WM_XBUTTONDBLCLK:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        if (!GetClientRect(hWndTarget, &stRect))
        {
            MessageBox(NULL, L"target lost!", L"VnhrDisplayWindow::WndProc", MB_OK);
            SendMessage(hWnd, WM_DESTROY, NULL, NULL);
        }
        _x = (double)x * ((double)stRect.right - stRect.left);
        _y = (double)y * ((double)stRect.bottom - stRect.top);
        GetClientRect(hWnd, &stRect);
        x = (WORD)(_x / ((double)stRect.right - stRect.left));
        y = (WORD)(_y / ((double)stRect.bottom - stRect.top));
        PostMessage(hWndTarget, message, wParam, MAKELPARAM(x, y));
        break;
    default:
        break;
    }
    return true;
}

LRESULT CALLBACK VnhrDisplayWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static WCHAR szBuffer[128];
    int x, y, i;
    double _x, _y;
    UINT idTimer;
    UINT display_delay_timerid;
    HRPROCESSTASK process_task;
    RECT stRect;
    BITMAPINFOHEADER* pbmih;
    HDC hDCTarget;
    HDC hDC;
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        hDC = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &stRect);
        //StretchDIBits(hDCDisplay, 0, 0, stRect.right - stRect.left, stRect.bottom - stRect.top, 0, 0, w, h, pBits, pbmi, DIB_RGB_COLORS, SRCCOPY);
        if (hDCDisplayCache_ && hBitmap_)
        {
            BitBlt(hDC, 0, 0, stRect.right - stRect.left, stRect.bottom - stRect.top, hDCDisplayCache_, 0, 0, SRCCOPY);
        }
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_TIMER:
    {
        if (!auto_mode_ or wParam != auto_timerid_)
            //wsprintf(szBuffer, L"killed idTimer %x", wParam);
            //MessageBox(NULL, szBuffer, NULL, MB_OK);
            KillTimer(hWnd, wParam);
            IDAllocator::GetIDAllocatorFor(IDALLOCATOR_TIMER)->DeallocateID(wParam);
        pbmih = img_capture_->CaptureAsBitmap(hWndTarget_);

        //wsprintf(szBuffer, L"captured bmp %x", pbmih);
        //MessageBox(NULL, szBuffer, NULL, MB_OK);

        process_task.hWnd = hWnd;
        process_task.pbmih = pbmih;
        GetClientRect(hWnd, &(process_task.stRectTo));
        //wsprintf(szBuffer, L"display window rect %d %d %d %d", 
        //    process_task.stRectTo.left,
        //    process_task.stRectTo.right,
        //    process_task.stRectTo.top,
        //    process_task.stRectTo.bottom);
        //MessageBox(NULL, szBuffer, NULL, MB_OK);
        GetClientRect(hWndTarget_, &(process_task.stRectFrom));
        //memcpy_s(&(process_task.stRectFrom), sizeof(RECT), &stRectFrom, sizeof(RECT));
        //memcpy_s(&(process_task.stRectTo), sizeof(RECT), &stRectTo, sizeof(RECT));

        processor_->ProcessHR(&process_task);
        //wsprintf(szBuffer, L"task sent %x", wParam);
        //MessageBox(NULL, szBuffer, NULL, MB_OK);
        break;
    }
    case VNHRM_FREE_BMP:
    {
        //wsprintf(szBuffer, L"receiving free bmp %x", hWnd);
        //MessageBox(NULL, szBuffer, L"VnhrDisplayWindow::WndProc", MB_OK);
        if (wParam)
            free((LPVOID)wParam);
        break;
    }
    case VNHRM_HRFINISHED:
    {
        //wsprintf(szBuffer, L"HR finished %x", hWnd);
        //MessageBox(NULL, szBuffer, L"VnhrDisplayWindow::WndProc", MB_OK);
        pbmih_ = (BITMAPINFOHEADER*)wParam;
        if (!pbmih_)
        {
            MessageBox(hWnd, L"Hyper Resolution failed!", L"VnhrDisplayWindow::WndProc", MB_OK);
            break;
        }
        if (!hDCDisplayCache_)
            break;
        hDC = GetDC(hWnd);
        hBitmap_ = CreateDIBitmap(hDC, pbmih_, CBM_INIT, ((BYTE*)pbmih_) + sizeof(BITMAPINFOHEADER), (BITMAPINFO*)pbmih_, DIB_RGB_COLORS);
        SelectObject(hDCDisplayCache_, hBitmap_);
        ReleaseDC(hWnd, hDC);
        //SetDIBitsToDevice(hDCDisplayCache_, 0, 0, pbmih_->biWidth, pbmih_->biHeight, 0, 0, 0, pbmih_->biHeight,
        //    ((BYTE*)pbmih_) + sizeof(BITMAPINFOHEADER), (BITMAPINFO*)pbmih_, DIB_RGB_COLORS);
        //SavePackedDIBtoFile(L"C:\\Users\\asus\\coding\\vsc++\\vnhr\\vnhr\\shortcut2.bmp", pbmih_);
        free(pbmih_);
        //GetClientRect(hWnd, &stRect);
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDC_AUTO:
        {
            if (!auto_mode_)
            {
                auto_timerid_ = IDAllocator::GetIDAllocatorFor(IDALLOCATOR_TIMER)->AllocateID();
                if (auto_timerid_ == -1)
                    break;
                //wsprintf(szBuffer, L"auto_timerid_ timer SET %x", auto_timerid_);
                //MessageBox(NULL, szBuffer, NULL, MB_OK);
                idTimer = SetTimer(hWnd, (UINT_PTR)auto_timerid_, uAutoInterval, VnhrDisplayWindow::DisplayWndTimerProc);
                auto_mode_ = true;
                //wsprintf(szBuffer, L"idTimer timer SET %x", idTimer);
                //MessageBox(NULL, szBuffer, NULL, MB_OK);
            }
            else
            {
                KillTimer(hWnd, auto_timerid_);
                IDAllocator::GetIDAllocatorFor(IDALLOCATOR_TIMER)->DeallocateID(auto_timerid_);
                auto_mode_ = false;
            }
            wsprintf(szBuffer, L"AUTO %d", (int)auto_mode_);
            MessageBox(hWnd, szBuffer, L"VnhrDisplayWindow::WndProc", MB_OK);
            break;
        }
        case IDM_EXIT:
        {
            Destruction();
            break;
        }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }
    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL:
    //case WM_CHAR:
    case WM_KEYDOWN:
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
        // create timers which won't replace each other
        display_delay_timerid = IDAllocator::GetIDAllocatorFor(IDALLOCATOR_TIMER)->AllocateID();
        if (display_delay_timerid == -1)
            break;
        //wsprintf(szBuffer, L"display_delay_timerid timer SET %x", display_delay_timerid);
        //MessageBox(NULL, szBuffer, NULL, MB_OK);
        //wsprintf(szBuffer, L"hWnd timer SET %x", hWnd);
        //MessageBox(NULL, szBuffer, NULL, MB_OK);
        idTimer = SetTimer(hWnd, (UINT_PTR)display_delay_timerid, uDisplayDelay, NULL);
        //wsprintf(szBuffer, L"idTimer timer SET %x", idTimer);
        //MessageBox(NULL, szBuffer, NULL, MB_OK);
    case WM_NCLBUTTONDOWN:
    case WM_NCRBUTTONDOWN:
    case WM_NCMBUTTONDOWN:
    case WM_NCXBUTTONDOWN:
    case WM_NCLBUTTONUP:
    case WM_NCRBUTTONUP:
    case WM_NCMBUTTONUP:
    case WM_NCXBUTTONUP:
    case WM_NCLBUTTONDBLCLK:
    case WM_NCRBUTTONDBLCLK:
    case WM_NCMBUTTONDBLCLK:
    case WM_NCXBUTTONDBLCLK:

    case WM_RBUTTONDBLCLK:
    case WM_LBUTTONDBLCLK:
    case WM_XBUTTONDBLCLK:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        //wsprintf(szBuffer, L"%d, %d", x, y);
        //MessageBox(NULL, szBuffer, NULL, MB_OK);
        //SetWindowText(hLabelTest, buffer);
        if (!GetClientRect(hWndTarget_, &stRect))
        {
            MessageBox(NULL, L"target lost!", L"VnhrDisplayWindow::WndProc", MB_OK);
            SendMessage(hWnd, WM_DESTROY, NULL, NULL);
        }
        _x = (double)x * ((double)stRect.right - stRect.left);
        _y = (double)y * ((double)stRect.bottom - stRect.top);
        GetClientRect(hWnd, &stRect);
        x = (WORD)(_x / ((double)stRect.right - stRect.left));
        y = (WORD)(_y / ((double)stRect.bottom - stRect.top));
        PostMessage(hWndTarget_, message, wParam, MAKELPARAM(x, y));
        //error = GetLastError();
        //wsprintf(buffer, L"%d", error);
        //SetWindowText(hLabelTest3, buffer);
        break;
    //case WM_CREATE:
    //    break;
    case WM_DESTROY:
        // seems that WM_DESTROY is not the last message, where to delete this?
        //delete this;
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}