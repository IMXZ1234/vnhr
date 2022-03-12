#pragma once
#include "framework.h"

class HRModel
{
public:
	HRModel() = default;
	HRModel(const HRModel&) = delete;
	virtual ~HRModel() = default;
	// Run Hyper Resolution.
	virtual BITMAPINFOHEADER* RunHRAsBitmap(const BITMAPINFOHEADER* pbmihFrom, const RECT* stRectFrom, const RECT* stRectTo) = 0;
};

