#pragma once
#include "framework.h"
#include <map>
#include <list>
#include <vector>
#include "hrcache.h"
#include "hrmodel.h"
#include "util.h"

#define HRTASK_PROCESS_OVER		1
#define HRTASK_APPEND			2
#define HRTASK_GET_FOR_PROCESS	3
#define HRTASK_CLEAR_ALL		4

#define VNHRM_FREE_BMP			0x501
#define VNHRM_HRFINISHED		0x502

#define HRPROCESS_DISCARDED		1
#define HRPROCESS_PROCESSED		2

struct HRPROCESSCONFIG
{
	HRModel* model;
	HRCache* cache;
};

struct HRPROCESSTASK
{
	HWND hWnd;
	BITMAPINFOHEADER* pbmih;
	RECT stRectFrom;
	RECT stRectTo;
};

class HRProcessor
{
public:
	static inline HRProcessor* GetInstance()
	{
		return &instance;
	}
	// Schedules the tasks and calls RunHR() at suitable time.
	// Will send notify message to hWndOwner when processing is complete.
	bool ProcessHR(const HRPROCESSTASK* pstTask);
	// Store the model and cache used by hWnd in internal map.
	bool Register(HWND hWnd, const HRPROCESSCONFIG* pstConfig);
	bool Unregister(HWND hWnd);
	bool SetMaxThreadNum(int max_thread_num);
private:
	static const HRPROCESSCONFIG* GetConfigFor(HWND hWnd);

	static HRProcessor instance;

	int max_thread_num_;
	int max_task_list_len_;
	std::map<HWND, HRPROCESSCONFIG*> config_map_;
	std::vector<HANDLE> thread_pool_;
	std::vector<DWORD> thread_ids_;
	std::list<HRPROCESSTASK*> task_waiting_list_;
	std::list<HRPROCESSTASK*> task_processing_list_;

	bool bThreadExit_;
	CRITICAL_SECTION stCS_;
	HANDLE hSemaphoreThread_;
	HANDLE hMutexAlter_;

	HRProcessor();
	~HRProcessor();

	// Manages task list.
	// Memory allocation/deallocation should be done outside this function.
	HRPROCESSTASK* AlterstTaskList(int op, HRPROCESSTASK* pstTask);
	// Thread function, does the Hyper Resolution work.
	static DWORD WINAPI RunHR(PVOID lParam);
};

