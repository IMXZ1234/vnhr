#pragma once
#include "framework.h"

class ImgCapture
{
public:
	virtual BITMAPFILEHEADER* CaptureAsBitmap(HWND hWnd) = 0;
};

