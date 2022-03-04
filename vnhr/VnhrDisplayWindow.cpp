#include "VnhrDisplayWindow.h"



DWORD WINAPI VnhrDisplayWindow::RunHR(PVOID lParam)
{
    CAPSTRUCT stCap;
    RECT stRect;
    int x, y;
    int i;
    double _x, _y;
    int w, h;
    BITMAP stBitmap;
    WCHAR szBuffer[128];
    WCHAR szBitmapFilePath[128];
    WCHAR szOutFilePath[128];
    WCHAR szOutConvertedFilePath[128];
    WCHAR szFileName[16];
    WCHAR szTemp[16];
    WCHAR szCmdLine[256];
    WIN32_FIND_DATA stFindData;
    HANDLE hFindFile;
    STARTUPINFO stStartUp;
    PROCESS_INFORMATION stProcInfo;
    BITMAPFILEHEADER* pbmfh;
    BITMAPINFO* pbmi;
    BITMAPINFOHEADER* pbmih;
    BITMAPCOREHEADER* pbmch;
    BYTE* pBits;
    while (true)
    {
        //MessageBox(NULL, L"RUN!", NULL, MB_OK);
        WaitForSingleObject(hSemaphoreThread, INFINITE);
        if (!AlterstCapList(&stCap, CAP_GET_FOR_PROCESS))
            // on exiting thread
            break;
        //MessageBox(NULL, L"AFTER SEMOPHORE!", NULL, MB_OK);
        if (!GetClientRect(hWndTarget, &stRect))
            DestroyWindow(hWndDisplay);
        //hDCDisplay = GetDC(hWndDisplay);
        //GetClientRect(hWndDisplay, &stRect);
        //BitBlt(hDCDisplay, 0, 0, stRect.right, stRect.bottom, stCap.hDC, 0, 0, SRCCOPY);
        //ReleaseDC(hWndDisplay, hDCDisplay);

        GetObject(stCap.hBitmap, sizeof(BITMAP), &stBitmap);
        GetCurrentDirectory(128, szCurrentDir);
        //MessageBox(NULL, szCurrentDir, L"szCurrentDir", MB_OK);
        i = -1;
        //find the first file name which is available to create .bmp(in other word, the path does not exist), 
        //as there may be multiple working threads creating files
        WaitForSingleObject(hMutexCreateFile, NULL);
        do {
            i++;
            szBitmapFilePath[0] = L'\0';
            wcscpy_s(szBitmapFilePath, szCurrentDir);
            wcscat_s(szBitmapFilePath, L"\\");
            wsprintf(szFileName, L"%d.bmp", i);
            wcscat_s(szBitmapFilePath, szFileName);
            hFindFile = FindFirstFile(szBitmapFilePath, &stFindData);
        } while (hFindFile != INVALID_HANDLE_VALUE);
        //MessageBox(NULL, szBitmapFilePath, L"szBitmapFilePath", MB_OK);
        // TODO
        SaveBitmapToFile(stCap.hDC, stCap.hBitmap, szBitmapFilePath);
        ReleaseMutex(hMutexCreateFile);

        DeleteDC(stCap.hDC);
        DeleteObject(stCap.hBitmap);

        //output file name
        wcscpy_s(szOutFilePath, szCurrentDir);
        wcscat_s(szOutFilePath, L"\\");
        wsprintf(szFileName, L"%d.png", i);
        wcscat_s(szOutFilePath, szFileName);
        //MessageBox(NULL, szOutFilePath, L"szOutFilePath", MB_OK);

        //output converted file name
        wcscpy_s(szOutConvertedFilePath, szCurrentDir);
        wcscat_s(szOutConvertedFilePath, L"\\");
        wsprintf(szFileName, L"%d_.bmp", i);
        wcscat_s(szOutConvertedFilePath, szFileName);
        //MessageBox(NULL, szOutConvertedFilePath, L"szOutConvertedFilePath", MB_OK);

        GetStartupInfo(&stStartUp);
        stStartUp.wShowWindow = SW_HIDE;
        stStartUp.dwFlags = stStartUp.dwFlags || STARTF_USESHOWWINDOW;

        //construct cmdline for HR exe
        wsprintf(szCmdLine, szCmdLineBase, szHRExePath, szBitmapFilePath, szOutFilePath, uHRScale, szModelrealesrgan_x4plus_anime);
        //MessageBox(NULL, szCmdLine, L"HR", MB_OK);
        ZeroMemory(&stProcInfo, sizeof(stProcInfo));
        if (CreateProcess(szHRExePath, szCmdLine, NULL, NULL, NULL, NORMAL_PRIORITY_CLASS, NULL, szHRExeDir, &stStartUp, &stProcInfo))
        {
            WaitForSingleObject(stProcInfo.hProcess, INFINITE);
            CloseHandle(stProcInfo.hProcess);
            CloseHandle(stProcInfo.hThread);
        }

        //construct cmdline for ffmpeg to extract bmp from png
        GetClientRect(hWndDisplay, &stRect);
        //wsprintf(szCmdLine, szffmpegBase, szffmpegExePath, szOutFilePath, szOutConvertedFilePath);
        wsprintf(szCmdLine, szffmpegResizeBase, szffmpegExePath, szOutFilePath, stRect.right - stRect.left, stRect.bottom - stRect.top, szOutConvertedFilePath);
        //MessageBox(NULL, szCmdLine, L"ffmpeg", MB_OK);
        ZeroMemory(&stProcInfo, sizeof(stProcInfo));
        if (CreateProcess(szffmpegExePath, szCmdLine, NULL, NULL, NULL, NORMAL_PRIORITY_CLASS, NULL, szHRExeDir, &stStartUp, &stProcInfo))
        {
            WaitForSingleObject(stProcInfo.hProcess, INFINITE);
            CloseHandle(stProcInfo.hProcess);
            CloseHandle(stProcInfo.hThread);
        }

        pbmfh = LoadDIBfromFile(szOutConvertedFilePath);
        if (!pbmfh)
        {
            MessageBox(NULL, L"Load Failed!", NULL, MB_OK);
            goto ret;
        }
        pbmi = (BITMAPINFO*)(pbmfh + 1);
        pbmih = (BITMAPINFOHEADER*)pbmi;
        if (pbmih->biSize == sizeof(BITMAPINFOHEADER))
        {
            w = pbmih->biWidth;
            h = abs(pbmih->biHeight);
        }
        else
        {
            pbmch = (BITMAPCOREHEADER*)pbmih;
            w = pbmch->bcWidth;
            h = abs(pbmch->bcHeight);
        }
        //wsprintf(szBuffer, L"%d %d", w, h);
        //MessageBox(NULL, szBuffer, NULL, MB_OK);
        pBits = (BYTE*)pbmfh + pbmfh->bfOffBits;
        hDCDisplay = GetDC(hWndDisplay);
        //GetClientRect(hWndDisplay, &stRect);
        //StretchDIBits(hDCDisplay, 0, 0, stRect.right - stRect.left, stRect.bottom - stRect.top, 0, 0, w, h, pBits, pbmi, DIB_RGB_COLORS, SRCCOPY);
        SetDIBitsToDevice(hDCDisplay, 0, 0, w, h, 0, 0, 0, h, pBits, pbmi, DIB_RGB_COLORS);
        ReleaseDC(hWndDisplay, hDCDisplay);
    ret:
        DeleteFile(szBitmapFilePath);
        DeleteFile(szOutFilePath);
        DeleteFile(szOutConvertedFilePath);
        AlterstCapList(&stCap, CAP_PROCESS_OVER);
    }
    return NULL;
}

LRESULT CALLBACK DisplayWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static WCHAR buffer[128];
    RECT stRect;
    HBITMAP hBMPCompat;
    CAPSTRUCT* pstCap;
    int x, y, i;
    double _x, _y;
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 分析菜单选择:
        switch (wmId)
        {
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
        AlterstCapList(NULL, CAP_CLEAR_ALL);
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