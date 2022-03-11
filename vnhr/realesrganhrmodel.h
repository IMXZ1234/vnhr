#pragma once
#include "hrmodel.h"
#include "util.h"

class RealesrganHRModel :
    public HRModel
{
public:
	RealesrganHRModel();
	~RealesrganHRModel();
	virtual BITMAPINFOHEADER* RunHRAsBitmap(const BITMAPINFOHEADER* pbmihFrom, const RECT* stRectFrom, const RECT* stRectTo);
private:
	HANDLE hMutexCreateFile_;
	int hr_scale;
};

