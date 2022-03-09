#pragma once
#include "framework.h"
#include <map>

class VnhrWindow
{
public:
	VnhrWindow();
	~VnhrWindow();
	inline VnhrWindow* GetObjectforWnd(HWND hWnd);
protected:
	std::map<HWND, VnhrWindow*> window_object_map;
};

