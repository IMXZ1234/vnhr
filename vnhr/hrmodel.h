#pragma once

class HRModel
{
public:
	// Run Hyper Resolution.
	// 
	virtual bool RunHRAsBitmap(BITMAPFILEHEADER* pbmihFrom, BITMAPFILEHEADER* pbmihTo);
};

