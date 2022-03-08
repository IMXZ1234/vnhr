#pragma once
#include "framework.h"

class HRModel
{
public:
	// Run Hyper Resolution.
	// 
	virtual bool RunHRAsBitmap(const BITMAPINFOHEADER* pbmihFrom, const RECT* stRectFrom, BITMAPINFOHEADER* pbmihTo, const RECT* stRectTo) = 0;
};

