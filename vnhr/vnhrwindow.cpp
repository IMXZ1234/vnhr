#include "vnhrwindow.h"

inline VnhrWindow* VnhrWindow::GetObjectforWnd(HWND hWnd)
{
	std::map<HWND, VnhrWindow*>::iterator it = window_object_map.find(hWnd);
	if (it != window_object_map.end())
		return (*it).second;
	return nullptr;
}
