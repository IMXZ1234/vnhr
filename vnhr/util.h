#pragma once
int wstrhex2int(WCHAR* lpwstr);

BITMAPFILEHEADER* LoadDIBfromFile(WCHAR* pszFilePath);
BOOL SaveDIBtoFile(WCHAR* pszFilePath, BITMAPFILEHEADER* pbmfh);
BOOL SavePackedDIBtoFile(WCHAR* pszFilePath, BITMAPINFOHEADER* pbmih);
void SaveBitmapToFile(HDC hDC, HBITMAP hBitmap, const WCHAR* path);
void GetDIBfromDDB(HDC hDC, HBITMAP hBitmap, const WCHAR* path);

