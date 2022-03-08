#pragma once
int wstrhex2int(const WCHAR* lpwstr);

bool GetDIBWidthHeight(BITMAPINFOHEADER* pbmih, int* width, int* height);

BITMAPFILEHEADER* LoadDIBfromFile(const WCHAR* pszFilePath);
BITMAPINFOHEADER* LoadPackedDIBfromFile(const WCHAR* pszFilePath);
bool LoadPackedDIBfromFileToBuffer(const WCHAR* pszFilePath, BITMAPINFOHEADER* pbmihTo, DWORD dwBufferSize);
bool SaveDIBtoFile(const WCHAR* pszFilePath, const BITMAPFILEHEADER* pbmfh);
bool SavePackedDIBtoFile(const WCHAR* pszFilePath, const BITMAPINFOHEADER* pbmih);

void SaveBitmapToFile(HDC hDC, HBITMAP hBitmap, const WCHAR* path);
void GetDIBfromDDB(HDC hDC, HBITMAP hBitmap, const WCHAR* path);

