#include "gdiimgcapture.h"

BITMAPFILEHEADER* GDIImgCapture::CaptureAsBitmap(HWND hWnd)
{
    BITMAP BitmapCap;
    HBITMAP hBitmapCompat;
    LPVOID lpBitmap = NULL;
    HANDLE hDIB = NULL;
    DWORD dwBmpSize = 0;
    RECT stRect;

    HDC hDC = GetDC(hWnd);
    HDC hDCCompat = CreateCompatibleDC(hDC);
    GetClientRect(hWnd, &stRect);
    int w = stRect.right - stRect.left;
    int h = stRect.bottom - stRect.top;
    hBitmapCompat = CreateCompatibleBitmap(hDC, w, h);
    SelectObject(hDCCompat, hBitmapCompat);
    BitBlt(hDCCompat, 0, 0, w, h, hDC, 0, 0, SRCCOPY);

    // Get the BITMAP from the HBITMAP.
    GetObject(hBitmapCompat, sizeof(BITMAP), &BitmapCap);

    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = w;
    bi.biHeight = h;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    dwBmpSize = ((BitmapCap.bmWidth * 32 + 31) / 32) * 4 * BitmapCap.bmHeight;

    // Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
    // call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
    // have greater overhead than HeapAlloc.
    lpBitmap = malloc(dwBmpSize);

    // Gets the "bits" from the bitmap, and copies them into a buffer 
    // that's pointed to by lpbitmap.
    GetDIBits(hDC, hBitmapCompat, 0,
        (UINT)BitmapCap.bmHeight,
        lpBitmap,
        (BITMAPINFO*)&bi, DIB_RGB_COLORS);
    ReleaseDC(hWnd, hDC);
    DeleteObject(hBitmapCompat);
    return (BITMAPFILEHEADER*)lpBitmap;
}
