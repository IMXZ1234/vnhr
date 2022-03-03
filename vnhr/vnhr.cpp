// vnhr.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "vnhr.h"
#include <list>
#include <vector>
//#include "opencv2/opencv.hpp"

#define MAX_LOADSTRING 100
using namespace std;

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWndClassMain[MAX_LOADSTRING];            // 主窗口类名
WCHAR szWndClassDisplay[MAX_LOADSTRING];
HWND hWndTarget;
HWND hWndDisplay;
HWND hEditWndTarget;
HWND hButtonStart;
HWND hLabelTest;
HWND hLabelTest2;
HWND hLabelTest3;
HDC hDCDisplay;
HDC hDCTarget;
HANDLE hThread;
DWORD dwThreadId;
BOOL bThreadExit;
CRITICAL_SECTION stCS;
HANDLE hMutexAlter;
HANDLE hMutexCreateFile;
HANDLE hSemaphoreThread;
DWORD error;
WCHAR buffer[128];
UINT uDisplayDelay = 400;
UINT ustCapCapacity = 1;
HDC hDCMem;
UINT uHRScale = 4;
UINT uScreenX, uScreenY;

WCHAR szCurrentDir[128];
WCHAR szHRExePath[] = L"C:\\Users\\asus\\Downloads\\Compressed\\realesrgan-ncnn-vulkan-20211212-windows\\realesrgan-ncnn-vulkan.exe";
WCHAR szffmpegExePath[] = L"C:\\Users\\asus\\coding\\vsc++\\ffmpeg-4.4.1-full_build-shared\\bin\\ffmpeg.exe";
WCHAR szHRExeDir[] = L"C:\\Users\\asus\\Downloads\\Compressed\\realesrgan-ncnn-vulkan-20211212-windows";
WCHAR szModelrealesrgan_x4plus_anime[] = L"realesrgan-x4plus-anime";
WCHAR szCmdLineBase[] = L"%s -i %s -o %s -s %d -n %s";
WCHAR szffmpegBase[] = L"%s -i %s %s";
WCHAR szffmpegResizeBase[] = L"%s -i %s -s %dx%d %s";
//WCHAR szffmpegBase[] = L"C:\Users\asus\coding\vsc++\ffmpeg-4.4.1-full_build-shared\bin\ffmpeg -i %s %s";

list<CAPSTRUCT*> pstCapList;
//capstruct under process
list<CAPSTRUCT*> pstCapProcessList;
//list<HBITMAP> hBMPList;
//list<HDC> hDCList;
//list<UINT_PTR> hIDTimerList;


DWORD baseUnit = GetDialogBaseUnits();
int cxChar = LOWORD(baseUnit);
int cyChar = HIWORD(baseUnit);

int wstrhex2int(WCHAR* lpwstr)
{
    int i = 0;
    int num = 0;
    WCHAR wcharsubtract;
    while (lpwstr[i] != L'\0')
    {
        if (L'a' <= lpwstr[i] && lpwstr[i] <= L'z')
            wcharsubtract = L'a' - 10;
        else if (L'A' <= lpwstr[i] && lpwstr[i] <= L'Z')
            wcharsubtract = L'A' - 10;
        else
            wcharsubtract = L'0';
        num = num * 16 + lpwstr[i] - wcharsubtract;
        i++;
    }
    return num;
}

BITMAPFILEHEADER* LoadDIBfromFile(WCHAR* pszFilePath)
{
    BOOL bSuccess;
    DWORD dwFileSize, dwHighSize, dwBytesRead;
    HANDLE hFile;
    BITMAPFILEHEADER* pbmfh;

    hFile = CreateFile(pszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return NULL;

    dwFileSize = GetFileSize(hFile, &dwHighSize);
    if (dwHighSize)
    {
        //file is too large
        CloseHandle(hFile);
        return NULL;
    }

    pbmfh = (BITMAPFILEHEADER*) malloc(dwFileSize);
    if (!pbmfh)
    {
        CloseHandle(hFile);
        return NULL;
    }

    bSuccess = ReadFile(hFile, pbmfh, dwFileSize, &dwBytesRead, NULL);
    CloseHandle(hFile);

    if (!bSuccess || (dwBytesRead != dwFileSize) || (pbmfh->bfType != 0x4D42) || (pbmfh->bfSize != dwFileSize))
    {
        MessageBox(NULL, L"Load bmp fail!", NULL, MB_OK);
        free(pbmfh);
        return NULL;
    }
    return pbmfh;
}

BOOL SaveDIBtoFile(WCHAR* pszFilePath, BITMAPFILEHEADER* pbmfh)
{
    BOOL bSuccess;
    DWORD dwBytesWritten;
    HANDLE hFile;

    hFile = CreateFile(pszFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
        return FALSE;

    bSuccess = WriteFile(hFile, pbmfh, pbmfh->bfSize, &dwBytesWritten, NULL);
    CloseHandle(hFile);

    if (!bSuccess || (dwBytesWritten != pbmfh->bfSize))
    {
        DeleteFile(pszFilePath);
        return FALSE;
    }
    return TRUE;
}

// 此代码模块中包含的函数的前向声明:
ATOM                VNHRMainClass(HINSTANCE hInstance);
ATOM                VNHRDisplayClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    DisplayWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                SaveBitmapToFile(HDC hDC, HBITMAP hBitmap, const WCHAR* path);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_VNHRMAIN, szWndClassMain, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_VNHRDISPLAY, szWndClassDisplay, MAX_LOADSTRING);
    VNHRMainClass(hInstance);
    VNHRDisplayClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VNHR));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM VNHRMainClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VNHR));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_VNHR);
    wcex.lpszClassName = szWndClassMain;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

ATOM VNHRDisplayClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = DisplayWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VNHR));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWndClassDisplay;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWndClassMain, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 300, 200, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


BOOL AlterstCapList(CAPSTRUCT* pstCap, DWORD op)
{
    CAPSTRUCT* pstCapTemp;
    WaitForSingleObject(hMutexAlter, NULL);
    switch (op)
    {
    case CAP_APPEND:
        //waiting list is at most ustCapCapacity long
        if (pstCapList.size() == ustCapCapacity)
        {
            // pop the oldest task
            free(pstCapList.front());
            pstCapList.pop_front();
        }
        pstCapTemp = (CAPSTRUCT*)malloc(sizeof(CAPSTRUCT));
        pstCapTemp->hBitmap = pstCap->hBitmap;
        pstCapTemp->hDC = pstCap->hDC;
        pstCapList.push_back(pstCapTemp);
        break;
    case CAP_PROCESS_OVER:
        //erase item from under process list
        //search for item which is the same as pstCap
        for (auto it = pstCapProcessList.cbegin(); it != pstCapProcessList.cend(); ++it)
        {
            pstCapTemp = *it;
            if (pstCapTemp->hBitmap == pstCap->hBitmap)
            {
                free(pstCapTemp);
                pstCapProcessList.erase(it);
                break;
            }
        }
        ReleaseMutex(hMutexAlter);
        return FALSE;
    case CAP_CLEAR_ALL:
        for (auto it = pstCapProcessList.cbegin(); it != pstCapProcessList.cend(); ++it)
            free(*it);
        for (auto it = pstCapList.cbegin(); it != pstCapList.cend(); ++it)
            free(*it);
        break;
    case CAP_GET_FOR_PROCESS:
        //some thread is going to work on this capstruct, 
        //move from waiting list to under process list
        if (pstCapList.size() != 0)
        {
            pstCapTemp = pstCapList.front();
            pstCap->hBitmap = pstCapTemp->hBitmap;
            pstCap->hDC = pstCapTemp->hDC;
            pstCapList.pop_front();
            pstCapProcessList.push_back(pstCapTemp);
            break;
        }
    default:
        ReleaseMutex(hMutexAlter); 
        return FALSE;
    }
    ReleaseMutex(hMutexAlter);
    return TRUE;
}

DWORD WINAPI RunHR(PVOID lParam)
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static WCHAR buffer[128];
    RECT stRect;
    HDC hDCScreen;
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case ID_BUTTONSTART:
                GetWindowText(hEditWndTarget, buffer, 128);
                hWndTarget = (HWND)wstrhex2int(buffer);
                if (!IsWindow(hWndTarget))
                {
                    wsprintf(buffer, L"Invalid Window Handle %X !", hWndTarget);
                    MessageBox(hWnd, buffer , L"hWndTarget", MB_OK);
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

void ViewhDCListLen()
{
    static WCHAR buffer[128];
    wsprintf(buffer, L"len %d", pstCapList.size());
    MessageBox(NULL, buffer, NULL, MB_OK);
}

void SaveBitmapToFile(HDC hDC, HBITMAP hBitmap, const WCHAR* path)
{
    BITMAP bmpScreen;
    DWORD dwBytesWritten = 0;
    DWORD dwSizeofDIB = 0;
    HANDLE hFile = NULL;
    char* lpbitmap = NULL;
    HANDLE hDIB = NULL;
    DWORD dwBmpSize = 0;

    BITMAPFILEHEADER   bmfHeader;
    BITMAPINFOHEADER   bi;

    // Get the BITMAP from the HBITMAP.
    GetObject(hBitmap, sizeof(BITMAP), &bmpScreen);

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpScreen.bmWidth;
    bi.biHeight = bmpScreen.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

    // Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
    // call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
    // have greater overhead than HeapAlloc.
    hDIB = GlobalAlloc(GHND, dwBmpSize);
    lpbitmap = (char*)GlobalLock(hDIB);

    // Gets the "bits" from the bitmap, and copies them into a buffer 
    // that's pointed to by lpbitmap.
    GetDIBits(hDC, hBitmap, 0,
        (UINT)bmpScreen.bmHeight,
        lpbitmap,
        (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    // A file is created, this is where we will save the screen capture.
    hFile = CreateFile(path,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);

    // Add the size of the headers to the size of the bitmap to get the total file size.
    dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // Offset to where the actual bitmap bits start.
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

    // Size of the file.
    bmfHeader.bfSize = dwSizeofDIB;

    // bfType must always be BM for Bitmaps.
    bmfHeader.bfType = 0x4D42; // BM.

    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

    // Unlock and Free the DIB from the heap.
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);

    // Close the handle for the file that was created.
    CloseHandle(hFile);
}

void GetDIBfromDDB(HDC hDC, HBITMAP hBitmap, const WCHAR* path)
{
    BITMAP bmpScreen;
    DWORD dwBytesWritten = 0;
    DWORD dwSizeofDIB = 0;
    HANDLE hFile = NULL;
    char* lpbitmap = NULL;
    HANDLE hDIB = NULL;
    DWORD dwBmpSize = 0;

    BITMAPFILEHEADER   bmfHeader;
    BITMAPINFOHEADER   bi;

    // Get the BITMAP from the HBITMAP.
    GetObject(hBitmap, sizeof(BITMAP), &bmpScreen);

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpScreen.bmWidth;
    bi.biHeight = bmpScreen.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

    // Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
    // call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
    // have greater overhead than HeapAlloc.
    hDIB = GlobalAlloc(GHND, dwBmpSize);
    lpbitmap = (char*)GlobalLock(hDIB);

    // Gets the "bits" from the bitmap, and copies them into a buffer 
    // that's pointed to by lpbitmap.
    GetDIBits(hDC, hBitmap, 0,
        (UINT)bmpScreen.bmHeight,
        lpbitmap,
        (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    // A file is created, this is where we will save the screen capture.
    hFile = CreateFile(path,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);

    // Add the size of the headers to the size of the bitmap to get the total file size.
    dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // Offset to where the actual bitmap bits start.
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

    // Size of the file.
    bmfHeader.bfSize = dwSizeofDIB;

    // bfType must always be BM for Bitmaps.
    bmfHeader.bfType = 0x4D42; // BM.

    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

    // Unlock and Free the DIB from the heap.
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);

    // Close the handle for the file that was created.
    CloseHandle(hFile);
}

//cv::Mat GetMatFromBitmap(HDC hDC, HBITMAP hBitmap)
//{
//    int x_size = 800, y_size = 600; // <-- Your res for the image
//    cv::Mat matBitmap; // <-- The image represented by mat
//    // Initialize DCs
//    HDC hdcSys = GetDC(NULL); // Get DC of the target capture..
//    HDC hdcMem = CreateCompatibleDC(hdcSys); // Create compatible DC 
//    void* ptrBitmapPixels; // <-- Pointer variable that will contain the potinter for the pixels
//    // Create hBitmap with Pointer to the pixels of the Bitmap
//    BITMAPINFO bi; 
//    HDC hdc;
//    ZeroMemory(&bi, sizeof(BITMAPINFO));
//    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//    bi.bmiHeader.biWidth = x_size;
//    bi.bmiHeader.biHeight = -y_size;  //negative so (0,0) is at top left
//    bi.bmiHeader.biPlanes = 1;
//    bi.bmiHeader.biBitCount = 32;
//
//    hdc = GetDC(NULL);
//    hBitmap = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &ptrBitmapPixels, NULL, 0);
//    // ^^ The output: hBitmap & ptrBitmapPixels
//    // Set hBitmap in the hdcMem 
//    SelectObject(hdcMem, hBitmap);
//    // Set matBitmap to point to the pixels of the hBitmap
//    matBitmap = cv::Mat(y_size, x_size, CV_8UC4, ptrBitmapPixels, 0);
//    //                ^^ note: first it is y, then it is x. very confusing
//    // * SETUP DONE *
//    // Now update the pixels using BitBlt
//    BitBlt(hdcMem, 0, 0, x_size, y_size, hdcSys, 0, 0, SRCCOPY);
//    cv::imshow("Title", matBitmap);
//    cv::waitKey(0);
//    return matBitmap;
//}


void CALLBACK DisplayWndTimerProc(HWND hWnd, UINT message, UINT_PTR iTimerID, DWORD dwTime)
{
    RECT stRect;
    HDC hDCCompat;
    HBITMAP hBitmapCompat;
    CAPSTRUCT stCap;
    WCHAR buffer[128];
    KillTimer(NULL, iTimerID);
    hDCTarget = GetDC(hWndTarget);
    hDCCompat = CreateCompatibleDC(hDCTarget);
    GetClientRect(hWndTarget, &stRect);
    hBitmapCompat = CreateCompatibleBitmap(hDCTarget, stRect.right - stRect.left, stRect.bottom - stRect.top);
    SelectObject(hDCCompat, hBitmapCompat);
    BitBlt(hDCCompat, 0, 0, stRect.right, stRect.bottom, hDCTarget, 0, 0, SRCCOPY);

    ReleaseDC(hWndTarget, hDCTarget);
    stCap.hBitmap = hBitmapCompat;
    stCap.hDC = hDCCompat;
    AlterstCapList(&stCap, CAP_APPEND);
    //ViewhDCListLen();
    ReleaseSemaphore(hSemaphoreThread, 1, NULL);
    //ViewhDCListLen();
    //MessageBox(hWnd, L"released!", NULL, MB_OK);
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

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
