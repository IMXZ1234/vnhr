#pragma once
#include "framework.h"

class HRModel
{
public:
	// Run Hyper Resolution.
	// 
	virtual BITMAPINFOHEADER* RunHRAsBitmap(const BITMAPINFOHEADER* pbmihFrom, const RECT* stRectFrom, const RECT* stRectTo) = 0;
};

