#include "framework.h"


int wstrhex2int(const WCHAR* lpwstr)
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

BITMAPFILEHEADER* LoadDIBfromFile(const WCHAR* pszFilePath)
{
    BOOL bSuccess;
    DWORD dwFileSize, dwHighSize, dwBytesRead;
    HANDLE hFile;
    BITMAPFILEHEADER* pbmfh;

    hFile = CreateFile(pszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return nullptr;

    dwFileSize = GetFileSize(hFile, &dwHighSize);
    if (dwHighSize)
    {
        //file is too large
        CloseHandle(hFile);
        return nullptr;
    }

    pbmfh = (BITMAPFILEHEADER*)malloc(dwFileSize);
    if (!pbmfh)
    {
        CloseHandle(hFile);
        return nullptr;
    }

    bSuccess = ReadFile(hFile, pbmfh, dwFileSize, &dwBytesRead, NULL);
    CloseHandle(hFile);

    if (!bSuccess || (dwBytesRead != dwFileSize) || (pbmfh->bfType != 0x4D42) || (pbmfh->bfSize != dwFileSize))
    {
        MessageBox(NULL, L"Load bmp fail!", NULL, MB_OK);
        free(pbmfh);
        return nullptr;
    }
    return pbmfh;
}


bool LoadPackedDIBfromFileToBuffer(const WCHAR* pszFilePath, BITMAPINFOHEADER* pbmihTo, DWORD dwBufferSize)
{
    return true;
}

bool GetDIBWidthHeight(BITMAPINFOHEADER* pbmih, int* width, int* height)
{
    BITMAPCOREHEADER* pbmch;
    if (pbmih->biSize == sizeof(BITMAPINFOHEADER))
    {
        *width = pbmih->biWidth;
        *height = abs(pbmih->biHeight);
    }
    else
    {
        pbmch = (BITMAPCOREHEADER*)pbmih;
        *width = pbmch->bcWidth;
        *height = abs(pbmch->bcHeight);
    }
    return true;
}

BITMAPINFOHEADER* LoadPackedDIBfromFile(const WCHAR* pszFilePath)
{
    BOOL bSuccess;
    DWORD dwFileSize, dwHighSize, dwBytesRead, dwPackedDIBSize;
    HANDLE hFile;
    BITMAPFILEHEADER* pbmfh;
    BITMAPINFOHEADER* pbmih;

    hFile = CreateFile(pszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return nullptr;

    dwFileSize = GetFileSize(hFile, &dwHighSize);
    if (dwHighSize)
    {
        //file is too large
        CloseHandle(hFile);
        return nullptr;
    }

    // load the header and see if bmp file is corrupted
    pbmfh = (BITMAPFILEHEADER*)malloc(sizeof(BITMAPFILEHEADER));
    if (!pbmfh)
    {
        CloseHandle(hFile);
        return nullptr;
    }
    bSuccess = ReadFile(hFile, pbmfh, sizeof(BITMAPFILEHEADER), &dwBytesRead, NULL);
    if (!bSuccess || (dwBytesRead != sizeof(BITMAPFILEHEADER)) || (pbmfh->bfType != 0x4D42) || (pbmfh->bfSize != dwFileSize))
    {
        MessageBox(NULL, L"Load bmp fail!", NULL, MB_OK);
        free(pbmfh);
        return nullptr;
    }
    free(pbmfh);

    // load the packed DIB which resides right after BITMAPFILEHEADER
    dwPackedDIBSize = dwFileSize - sizeof(BITMAPFILEHEADER);
    pbmih = (BITMAPINFOHEADER*)malloc(dwPackedDIBSize);
    if (!pbmih)
    {
        CloseHandle(hFile);
        return nullptr;
    }
    bSuccess = ReadFile(hFile, pbmih, dwPackedDIBSize, &dwBytesRead, NULL);
    CloseHandle(hFile);
    if (!bSuccess || (dwBytesRead != dwFileSize))
    {
        MessageBox(NULL, L"Load bmp fail!", NULL, MB_OK);
        free(pbmih);
        return nullptr;
    }
    return pbmih;
}

bool SaveDIBtoFile(const WCHAR* pszFilePath, const BITMAPFILEHEADER* pbmfh)
{
    BOOL bSuccess;
    DWORD dwBytesWritten;
    HANDLE hFile;

    hFile = CreateFile(pszFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    bSuccess = WriteFile(hFile, pbmfh, pbmfh->bfSize, &dwBytesWritten, NULL);
    CloseHandle(hFile);

    if (!bSuccess || (dwBytesWritten != pbmfh->bfSize))
    {
        DeleteFile(pszFilePath);
        return false;
    }
    return true;
}

bool SavePackedDIBtoFile(const WCHAR* pszFilePath, const BITMAPINFOHEADER* pbmih)
{
    BOOL bSuccess;
    DWORD dwBytesWritten;
    HANDLE hFile;
    BITMAPFILEHEADER bmfh;

    DWORD dwBmpSize = ((pbmih->biWidth * pbmih->biBitCount + 31) / 32) * 4 * pbmih->biHeight;
    DWORD dwPackedDIBSize = dwBmpSize + sizeof(BITMAPINFOHEADER);
    // Offset to where the actual bitmap bits start.
    bmfh.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
    // Add the size of the headers to the size of the bitmap to get the total file size.
    // Size of the file.
    bmfh.bfSize = dwPackedDIBSize + sizeof(BITMAPFILEHEADER);
    // bfType must always be BM for Bitmaps.
    bmfh.bfType = 0x4D42; // BM.

    hFile = CreateFile(pszFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    // Write the file header.
    bSuccess = WriteFile(hFile, &bmfh, (DWORD)sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    if (!bSuccess || (dwBytesWritten != (DWORD)sizeof(BITMAPFILEHEADER)))
    {
        DeleteFile(pszFilePath);
        return false;
    }
    // Write packed DIB. 
    bSuccess = WriteFile(hFile, &pbmih, dwPackedDIBSize, &dwBytesWritten, NULL);
    if (!bSuccess || (dwBytesWritten != dwPackedDIBSize))
    {
        DeleteFile(pszFilePath);
        return false;
    }

    CloseHandle(hFile);
    return true;
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