#include "hrprocessor.h"

inline HRProcessor HRProcessor::GetInstance()
{
    return instance_;
}

bool HRProcessor::ProcessHR(HWND hWnd, const HRPROCESSTASK* pstTask)
{
    if (pstTask == nullptr)
        return false;
    HRPROCESSTASK* target = (HRPROCESSTASK*)malloc(sizeof(HRPROCESSTASK));
    memcpy_s(target, sizeof(HRPROCESSTASK), pstTask, sizeof(HRPROCESSTASK));
    HRPROCESSTASK* ret = AlterstTaskList(HRTASK_APPEND, target);
    if (ret != nullptr)
    {
        PostMessage(hWnd, VNHRM_FREE_BMP, (WPARAM)ret->pbmih, HRPROCESS_DISCARDED);
        free(ret);
    }
    return true;
}

bool HRProcessor::Register(HWND hWnd, const HRPROCESSCONFIG* pstConfig)
{
    std::map<HWND, HRPROCESSCONFIG*>::iterator it = config_map_.find(hWnd);
    HRPROCESSCONFIG* config;
    if (it != config_map_.end())
    {
        config = (*it).second;
        config->cache = pstConfig->cache;
        config->model = pstConfig->model;
    }
    else
    {
        config = (HRPROCESSCONFIG*)malloc(sizeof(HRPROCESSCONFIG));
        config->cache = pstConfig->cache;
        config->model = pstConfig->model;
        config_map_.insert(std::pair<HWND, HRPROCESSCONFIG*>(hWnd, config));
    }
    return true;
}

bool HRProcessor::Unregister(HWND hWnd)
{
    std::map<HWND, HRPROCESSCONFIG*>::iterator it = config_map_.find(hWnd);
    if (it != config_map_.end())
    {
        free((*it).second);
        config_map_.erase(it);
        return true;
    }
    return false;
}

bool HRProcessor::SetMaxThreadNum(int max_thread_num)
{
    max_thread_num_ = max_thread_num;
    return true;
}

HRPROCESSTASK* HRProcessor::AlterstTaskList(int op, HRPROCESSTASK* pstTask)
{
    std::list<HRPROCESSTASK*>::const_iterator it;
    HRPROCESSTASK* target = nullptr;
    WaitForSingleObject(hMutexAlter_, NULL);
    switch (op)
    {
    case HRTASK_APPEND:
        //waiting list is at most ustCapCapacity long
        if (task_waiting_list_.size() == max_task_list_len_)
        {
            // pop the oldest task
            target = task_waiting_list_.front();
            task_waiting_list_.pop_front();
        }
        task_processing_list_.push_back(pstTask);
        break;
    case HRTASK_PROCESS_OVER:
        //erase item from under process list
        //search for item which is the same as pstCap
        for (it = task_processing_list_.cbegin(); it != task_processing_list_.cend(); ++it)
        {
            if (pstTask == *it)
            {
                task_processing_list_.erase(it);
                break;
            }
        }
        break;
    case HRTASK_CLEAR_ALL:
        for (it = task_processing_list_.cbegin(); it != task_processing_list_.cend(); ++it)
            free(*it);
        for (it = task_waiting_list_.cbegin(); it != task_waiting_list_.cend(); ++it)
            free(*it);
        break;
    case HRTASK_GET_FOR_PROCESS:
        //some thread is going to work on this capstruct, 
        //move from waiting list to under process list
        if (task_waiting_list_.size() != 0)
        {
            target = task_waiting_list_.front();
            task_waiting_list_.pop_front();
            task_processing_list_.push_back(target);
            break;
        }
    default:
        break;
    }
    ReleaseMutex(hMutexAlter_);
    return target;
}


DWORD WINAPI HRProcessor::RunHR(PVOID lParam)
{
    HRPROCESSTASK* stCap;
    RECT stRect;
    int x, y;
    int i;
    double _x, _y;
    int w, h;
    BITMAP stBitmap;
    WCHAR szCurrentDir[128];
    WCHAR szBuffer[128];
    WCHAR szBitmapFilePath[128];
    WCHAR szOutFilePath[128];
    WCHAR szOutConvertedFilePath[128];
    WCHAR szFileName[16];
    WCHAR szTemp[16];
    WCHAR szCmdLine[256];
    WIN32_FIND_DATA stFindData;
    HANDLE hFindFile;
    STARTUPINFO stStartUp;
    PROCESS_INFORMATION stProcInfo;
    BITMAPFILEHEADER* pbmfh;
    BITMAPINFO* pbmi;
    BITMAPINFOHEADER* pbmih;
    BITMAPCOREHEADER* pbmch;
    BYTE* pBits;
    while (true)
    {
        //MessageBox(NULL, L"RUN!", NULL, MB_OK);
        WaitForSingleObject(instance_.hSemaphoreThread_, INFINITE);
        stCap = instance_.AlterstTaskList(HRTASK_GET_FOR_PROCESS, nullptr);
        if (stCap == nullptr)
            // on exiting thread
            break;

        
        instance_.AlterstTaskList(HRTASK_PROCESS_OVER, stCap);
    }
    return NULL;
}
