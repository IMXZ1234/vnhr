#include "vnhrmainwindow.h"

const WCHAR VnhrMainWindow::szWndClassName_[32] = TEXT("VnhrMainWindow");

ATOM VnhrMainWindow::RegisterWndClass(HINSTANCE hInstance)
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
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_VNHR);
    wcex.lpszClassName = VnhrMainWindow::szWndClassName_;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

bool VnhrMainWindow::Init(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    bSearchingTarget = false;
    hEdithWndTarget = NULL;
    hButtonStart = NULL;
    hStaticSearchTarget = NULL;
    MessageBox(NULL, L"MAIN", NULL , MB_OK);
    return VnhrWindow::Init(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

LRESULT CALLBACK VnhrMainWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WCHAR szBuffer[128];
    DWORD baseUnit;
    HDC hDCScreen;
    HWND hWndTarget;
    int uScreenX, uScreenY;
    int cxChar, cyChar;
    POINT point;
    static HWND hLastWndUnderMouse = NULL;
    static HWND hWndUnderMouse = NULL;
    VnhrDisplayWindow* display_window;
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 分析菜单选择:
        switch (wmId)
        {
        case ID_BUTTONSTART:
            GetWindowText(hEdithWndTarget, szBuffer, 128);
            hWndTarget = (HWND)wstrhex2int(szBuffer);
            if (!IsWindow(hWndTarget))
            {
                wsprintf(szBuffer, L"Invalid Window Handle %X !", hWndTarget);
                MessageBox(hWnd, szBuffer, L"hWndTarget", MB_OK);
                break;
            }
            hDCScreen = GetDC(NULL);
            uScreenX = GetDeviceCaps(hDCScreen, HORZRES);
            uScreenY = GetDeviceCaps(hDCScreen, VERTRES);
            ReleaseDC(NULL, hDCScreen);
            display_window = new VnhrDisplayWindow;
            //GetWindowText(hWndTarget, szBuffer, 128);
            //MessageBox(hWnd, szBuffer, L"hWndTarget title", MB_OK);
            display_window->Init(NULL, VnhrDisplayWindow::szWndClassName_, szBuffer, WS_VISIBLE, 0, 0,
                uScreenX, uScreenY, hWnd, NULL, hInstance_, NULL);
            display_window->set_target_window(hWndTarget);
            MessageBox(hWnd, L"display Init complete", L"hWndTarget", MB_OK);
            break;
        case ID_STATICSEARCHTARGET:
            //MessageBox(hWnd, L"Static clicked", NULL, MB_OK); | WS_POPUP
            bSearchingTarget = TRUE;
            hWndUnderMouse = NULL;
            hLastWndUnderMouse = NULL;
            SetCapture(hWnd);
            break;
        case IDM_ABOUT:
            DialogBox(hInstance_, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, VnhrMainWindow::About);
            break;
        case IDM_EXIT:
            Destruction();
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_MOUSEMOVE:
        if (bSearchingTarget && wParam == MK_LBUTTON)
        {
            GetCursorPos(&point);
            //point.x = GET_X_LPARAM(lParam);
            //point.y = GET_Y_LPARAM(lParam);
            hWndUnderMouse = WindowFromPoint(point);
            if (hWndUnderMouse != hLastWndUnderMouse)
            {
                wsprintf(szBuffer, L"%X", hWndUnderMouse);
                SetWindowText(hEdithWndTarget, szBuffer);
                wsprintf(szBuffer, L"x: %d y: %d", point.x, point.y);
                SetWindowText(hStaticSearchTarget, szBuffer);
                hLastWndUnderMouse = hWndUnderMouse;
            }
        }
        break;
    case WM_LBUTTONUP:
        ReleaseCapture();
        bSearchingTarget = FALSE;
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 在此处添加使用 hdc 的任何绘图代码...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_CREATE:
        baseUnit = GetDialogBaseUnits();
        cxChar = LOWORD(baseUnit);
        cyChar = HIWORD(baseUnit);
        // MessageBox(hWnd, L"create", NULL, MB_OK);
        hEdithWndTarget = CreateWindow(TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE, cxChar, cyChar / 2, cxChar * 20, cyChar, hWnd, (HMENU)ID_EDITHWNDTARTGET, hInstance_, NULL);
        hStaticSearchTarget = CreateWindow(TEXT("STATIC"), TEXT("Find\nWindow"), WS_CHILD | WS_VISIBLE | SS_NOTIFY, cxChar, cyChar * 3 / 2, cxChar * 8, cyChar * 4, hWnd, (HMENU)ID_STATICSEARCHTARGET, hInstance_, NULL);
        hButtonStart = CreateWindow(TEXT("BUTTON"), NULL, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, cxChar, cyChar * 6, cxChar * 20, cyChar *7 / 4, hWnd, (HMENU)ID_BUTTONSTART, hInstance_, NULL);
        break;
    case WM_DESTROY:
        delete this;
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK VnhrMainWindow::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
