#include "VnhrDisplayWindow.h"

const WCHAR VnhrDisplayWindow::szWndClassName_[32] = TEXT("VnhrDisplayWindow");
IDAllocator VnhrDisplayWindow::timer_idallocator(0, -1);

void CALLBACK Timerproc(
    HWND hWnd,
    UINT unnamedParam2,
    UINT_PTR idTimer,
    DWORD unnamedParam4
)
{
    WCHAR szBuffer[128];
    wsprintf(szBuffer, L"in timer proc %x", idTimer);
    MessageBox(NULL, szBuffer, NULL, MB_OK);
    KillTimer(hWnd, idTimer);
}

void CALLBACK DisplayWndTimerProc(HWND hWnd, UINT message, UINT_PTR iTimerID, DWORD dwTime)
{
    RECT stRect;
    HDC hDCCompat;
    HBITMAP hBitmapCompat;
    CAPSTRUCT stCap;
    WCHAR buffer[128];
    KillTimer(NULL, iTimerID);
    hDCTarget = GetDC(hWndTarget_);
    hDCCompat = CreateCompatibleDC(hDCTarget);
    GetClientRect(hWndTarget, &stRect);
    hBitmapCompat = CreateCompatibleBitmap(hDCTarget, stRect.right - stRect.left, stRect.bottom - stRect.top);
    SelectObject(hDCCompat, hBitmapCompat);
    BitBlt(hDCCompat, 0, 0, stRect.right, stRect.bottom, hDCTarget, 0, 0, SRCCOPY);

    ReleaseDC(hWndTarget, hDCTarget);
    stCap.hBitmap = hBitmapCompat;
    stCap.hDC = hDCCompat;
    AlterstCapList(&stCap, HRTASK_APPEND);
    //ViewhDCListLen();
    ReleaseSemaphore(hSemaphoreThread, 1, NULL);
    //ViewhDCListLen();
    //MessageBox(hWnd, L"released!", NULL, MB_OK);
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
    uDisplayDelay = 400;
    hWndTarget_ = NULL;
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

inline bool VnhrDisplayWindow::SetTargetWindow(HWND hWndTarget)
{
    hWndTarget_ = hWndTarget;
    return true;
}

inline HWND VnhrDisplayWindow::GetTargetWindow()
{
    return hWndTarget_;
}

LRESULT CALLBACK VnhrDisplayWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static WCHAR szBuffer[128];
    RECT stRect;
    HBITMAP hBMPCompat;
    int x, y, i;
    double _x, _y;
    VnhrDisplayWindow* obj = (VnhrDisplayWindow*)GetObjectforWnd(hWnd);
    int available_timerid;
    UINT idTimer;
    switch (message)
    {
    case WM_TIMER:
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
        case VNHRM_FREE_BMP:
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        case IDC_AUTO:
            MessageBox(hWnd, L"Install", NULL, MB_OK);
            available_timerid = timer_idallocator.AllocateID();
            if (available_timerid == -1)
                break;
            idTimer = SetTimer(hWnd, available_timerid, 1000, Timerproc);
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
        SetDIBitsToDevice(hdc, 0, 0, w, h, 0, 0, 0, h, pBits, pbmi, DIB_RGB_COLORS);
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
        if (!GetClientRect(hWndTarget, &stRect))
            SendMessage(hWndDisplay, WM_DESTROY, NULL, NULL);
        _x = (double)x * (double)(stRect.right - stRect.left);
        _y = (double)y * (double)(stRect.bottom - stRect.top);
        GetClientRect(hWnd, &stRect);
        x = (WORD)(_x / (double)(stRect.right - stRect.left));
        y = (WORD)(_y / (double)(stRect.bottom - stRect.top));
        PostMessage(hWndTarget, message, wParam, MAKELPARAM(x, y));
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