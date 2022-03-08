#include "vnhrmainwindow.h"


LRESULT CALLBACK VnhrMainWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static WCHAR buffer[128];
    RECT stRect;
    HDC hDCScreen;
    HWND hWndTarget;
    int uScreenX, uScreenY;
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
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}