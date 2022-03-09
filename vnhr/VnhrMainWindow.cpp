#include "vnhrmainwindow.h"

HWND hButtonFindApp;
HWND hButtonInstallHook;
HWND hStaticInstallHook;
HWND hEdithWndTarget;
BOOL bHookInstalled = FALSE;
BOOL bSearchingTarget = FALSE;
HMODULE hDllhk;
HANDLE hTimer;
UINT idTimer = NULL;
std::map<int, UINT> idTimerMap;

int GetAvailableTimerID()
{
    int i;
    std::map<int, UINT>::iterator it;
    for (i = 0; i < idTimerMap.max_size(); i++)
    {
        it = idTimerMap.find(i);
        if (it != idTimerMap.end())
            return idTimerMap.at(i);
    }
    return -1;
}

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

LRESULT CALLBACK VnhrMainWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static WCHAR buffer[128];
    RECT stRect;
    HDC hDCScreen;
    HWND hWndTarget;
    int uScreenX, uScreenY;
    POINT point;
    HWND hLastWndUnderMouse = NULL;
    HWND hWndUnderMouse = NULL;
    WCHAR szBuffer[128];
    int available_timerid;
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 分析菜单选择:
        switch (wmId)
        {
        case ID_BUTTONSTART:
            GetWindowText(instance_.hEditWndTarget, buffer, 128);
            hWndTarget = (HWND)wstrhex2int(buffer);
            if (!IsWindow(hWndTarget))
            {
                wsprintf(buffer, L"Invalid Window Handle %X !", hWndTarget);
                MessageBox(hWnd, buffer, L"hWndTarget", MB_OK);
                break;
            }
            //GetClientRect(hWndTarget, &stRect);
            //hWndDisplay = CreateWindow(szWndClassDisplay, buffer, WS_VISIBLE | WS_OVERLAPPEDWINDOW, 0, 0, 
            //    stRect.right - stRect.left, stRect.bottom - stRect.top, hWnd, NULL, hInst, NULL);
            hDCScreen = GetDC(NULL);
            uScreenX = GetDeviceCaps(hDCScreen, HORZRES);
            uScreenY = GetDeviceCaps(hDCScreen, VERTRES);
            ReleaseDC(NULL, hDCScreen);
            hWndDisplay = CreateWindow(szWndClassDisplay, buffer, WS_VISIBLE | WS_POPUP, 0, 0,
                uScreenX, uScreenY, hWnd, NULL, hInst, NULL);

            break;
        case IDC_BUTTONINSTALLHOOK:
            MessageBox(hWnd, L"Install", NULL, MB_OK);
            available_timerid = GetAvailableTimerID();
            if (available_timerid == -1)
                break;
            idTimer = SetTimer(hWnd, available_timerid, 1000, Timerproc);
            wsprintf(szBuffer, L"timer SET %x", idTimer);
            MessageBox(NULL, szBuffer, NULL, MB_OK);
            //hDllhk = LoadLibrary(L"hk.dll");
            break;
        case IDC_STATICINSTALLHOOK:
            //MessageBox(hWnd, L"Static clicked", NULL, MB_OK);
            bSearchingTarget = TRUE;
            hWndUnderMouse = NULL;
            hLastWndUnderMouse = NULL;
            SetCapture(hWnd);
            break;
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
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
                SetWindowText(hStaticInstallHook, szBuffer);
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
        // MessageBox(hWnd, L"create", NULL, MB_OK);
        hEditWndTarget = CreateWindow(TEXT("EDIT"), NULL, WS_VISIBLE | WS_CHILD | BS_TEXT | ES_NUMBER, cxChar, cyChar, 20 * cxChar, cyChar * 7 / 4, hWnd, (HMENU)ID_EDITWNDTARTGET, hInst, NULL);
        hButtonStart = CreateWindow(TEXT("BUTTON"), NULL, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, cxChar, cyChar * 2, 20 * cxChar, cyChar * 7 / 4, hWnd, (HMENU)ID_BUTTONSTART, hInst, NULL);
        hButtonFindApp = CreateWindow(L"BUTTON", L"FindAppLabel", WS_CHILD | WS_VISIBLE, 10, 10, 60, 20, hWnd, (HMENU)IDC_BUTTONFINDAPP, hInst, NULL);
        hButtonInstallHook = CreateWindow(L"BUTTON", L"InstallHook!", WS_CHILD | WS_VISIBLE, 10, 35, 60, 20, hWnd, (HMENU)IDC_BUTTONINSTALLHOOK, hInst, NULL);
        hStaticInstallHook = CreateWindow(L"STATIC", L"staticInstallHook!", WS_CHILD | WS_VISIBLE | SS_NOTIFY, 10, 60, 180, 20, hWnd, (HMENU)IDC_STATICINSTALLHOOK, hInst, NULL);
        hEdithWndTarget = CreateWindow(L"EDIT", L" ", WS_CHILD | WS_VISIBLE, 10, 85, 60, 20, hWnd, (HMENU)IDC_EDITHWNDTARGET, hInst, NULL);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}