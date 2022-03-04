#pragma once
#include "framework.h"
#include <map.h>
#define CAP_PROCESS_OVER	1
#define CAP_APPEND			2
#define CAP_GET_FOR_PROCESS 3
#define CAP_CLEAR_ALL		4


struct CAPSTRUCT
{
	HDC hDC;
	HBITMAP hBitmap;
};

struct HRPROCESSDATA
{
	CAPSTRUCT stCap;
	RECT stRectFrom;
	RECT stRectTo;
};

struct HRPROCESSCONFIG
{
	HRModel* model;
	HRCache* 
};

class HRProcessor
{
public:
	HRProcessor GetInstance()
	{
		return instance_;
	}
	// Will send notify message to hWndOwner when processing is complete.
	bool ProcessHR(HWND hWndOwner, HRPROCESSDATA* data);
	bool RegisterModel();
private:
	static HRProcessor instance_;
	int max_thread_num = 1;
	int max_task_list_len = 1;
	HRProcessor() = default;
};

