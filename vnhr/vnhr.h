#pragma once

#include "framework.h"
#include "resource.h"

#define ID_EDITWNDTARTGET	1
#define ID_BUTTONSTART		2
#define ID_LABELTEST		3

#define ID_TIMERDISPLAYDELAY 1

#define CAP_PROCESS_OVER	1
#define CAP_APPEND			2
#define CAP_GET_FOR_PROCESS 3
#define CAP_CLEAR_ALL		4



struct CAPSTRUCT
{
	HDC hDC;
	HBITMAP hBitmap;
};
