#include "VnhrDisplayWindow.h"


LRESULT CALLBACK VnhrDisplayWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static WCHAR buffer[128];
    RECT stRect;
    HBITMAP hBMPCompat;
    int x, y, i;
    double _x, _y;
    VnhrDisplayWindow* pInstance = (VnhrDisplayWindow*)GetObjectforWnd(hWnd);
    switch (message)
    {
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
        SetTimer(NULL, 0, uDisplayDelay, DisplayWndTimerProc);
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
        error = PostMessage(hWndTarget, message, wParam, MAKELPARAM(x, y));
        //error = GetLastError();
        //wsprintf(buffer, L"%d", error);
        //SetWindowText(hLabelTest3, buffer);
        break;
    case WM_CREATE:
        bThreadExit = false;
        hSemaphoreThread = CreateSemaphore(NULL, 0, ustCapCapacity, NULL);
        hThread = CreateThread(NULL, 0, RunHR, NULL, 0, &dwThreadId);
        hMutexAlter = CreateMutex(NULL, FALSE, NULL);
        hMutexCreateFile = CreateMutex(NULL, FALSE, NULL);
        //hLabelTest = CreateWindow(TEXT("STATIC"), NULL, WS_VISIBLE | WS_CHILD, cxChar, cyChar, 20 * cxChar, cyChar * 7 / 4, hWnd, (HMENU)ID_LABELTEST, hInst, NULL);
        //hLabelTest2 = CreateWindow(TEXT("STATIC"), NULL, WS_VISIBLE | WS_CHILD, cxChar, cyChar * 2, 20 * cxChar, cyChar * 7 / 4, hWnd, (HMENU)ID_LABELTEST, hInst, NULL);
        //hLabelTest3 = CreateWindow(TEXT("STATIC"), NULL, WS_VISIBLE | WS_CHILD, cxChar, cyChar * 3, 20 * cxChar, cyChar * 7 / 4, hWnd, (HMENU)ID_LABELTEST, hInst, NULL);
        break;
    case WM_DESTROY:
        TerminateThread(hThread, 0);
        for (auto it = pstCapProcessList.cbegin(); it != pstCapProcessList.cend(); ++it)
        {
            pstCap = *it;
            DeleteObject(pstCap->hBitmap);
            DeleteDC(pstCap->hDC);
        }
        for (auto it = pstCapList.cbegin(); it != pstCapList.cend(); ++it)
        {
            pstCap = *it;
            DeleteObject(pstCap->hBitmap);
            DeleteDC(pstCap->hDC);
        }
        AlterstCapList(NULL, HRTASK_CLEAR_ALL);
        //// wait for running threads to finish
        //SwitchToThread();
        CloseHandle(hThread);
        CloseHandle(hMutexAlter);
        CloseHandle(hMutexCreateFile);
        CloseHandle(hSemaphoreThread);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}