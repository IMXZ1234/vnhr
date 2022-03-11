#pragma once
#include "imgcapture.h"
class GDIImgCapture :
    public ImgCapture
{
public:
	virtual BITMAPINFOHEADER* CaptureAsBitmap(HWND hWnd);
};

