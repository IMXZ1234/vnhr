#pragma once
#include "imgcapture.h"
class GDIImgCapture :
    public ImgCapture
{
public:
	virtual BITMAPFILEHEADER* CaptureAsBitmap(HWND hWnd);
};

