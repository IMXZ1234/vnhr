#pragma once
#include "hrmodel.h"
#include "util.h"

class RealesrganHRModel :
    public HRModel
{
public:
	RealesrganHRModel();
	~RealesrganHRModel();
	virtual bool RunHRAsBitmap(const BITMAPINFOHEADER* pbmihFrom, const RECT* stRectFrom, BITMAPINFOHEADER* pbmihTo, const RECT* stRectTo);
private:
	HANDLE hSemaphoreThread_;
	HANDLE hMutexAlter_;
	HANDLE hMutexCreateFile_;
	int hr_scale;
};

