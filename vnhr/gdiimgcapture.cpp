#include "gdiimgcapture.h"

BITMAPINFOHEADER* GDIImgCapture::CaptureAsBitmap(HWND hWnd)
{
    BITMAP BitmapCap;
    HBITMAP hBitmapCompat;
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

    dwBmpSize = ((BitmapCap.bmWidth * 32 + 31) / 32) * 4 * BitmapCap.bmHeight;

    BITMAPINFOHEADER* pbmih = (BITMAPINFOHEADER*)malloc(dwBmpSize + sizeof(BITMAPINFOHEADER));
    pbmih->biSize = sizeof(BITMAPINFOHEADER);
    pbmih->biWidth = w;
    pbmih->biHeight = h;
    pbmih->biPlanes = 1;
    pbmih->biBitCount = 32;
    pbmih->biCompression = BI_RGB;
    pbmih->biSizeImage = 0;
    pbmih->biXPelsPerMeter = 0;
    pbmih->biYPelsPerMeter = 0;
    pbmih->biClrUsed = 0;
    pbmih->biClrImportant = 0;

    // Gets the "bits" from the bitmap, and copies them into a buffer 
    // that's pointed to by lpbitmap.
    GetDIBits(hDC, hBitmapCompat, 0,
        (UINT)BitmapCap.bmHeight,
        (BYTE*)pbmih + sizeof(BITMAPINFOHEADER),
        (BITMAPINFO*)&pbmih, DIB_RGB_COLORS);
    ReleaseDC(hWnd, hDC);
    DeleteObject(hBitmapCompat);
    return pbmih;
}
