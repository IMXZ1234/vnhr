#pragma once
#include "framework.h"

class ImgCapture
{
public:
	virtual BITMAPINFOHEADER* CaptureAsBitmap(HWND hWnd) = 0;
};

