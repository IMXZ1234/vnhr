#include "hrprocessor.h"

inline HRProcessor HRProcessor::GetInstance()
{
    return instance_;
}

bool HRProcessor::ProcessHR(HWND hWnd, HRPROCESSTASK* pstTask)
{
    if (pstTask == nullptr)
        return false;
    HRPROCESSTASK* target = (HRPROCESSTASK*)malloc(sizeof(HRPROCESSTASK));
    memcpy_s(target, sizeof(HRPROCESSTASK), pstTask, sizeof(HRPROCESSTASK));
    HRPROCESSTASK* ret = AlterstTaskList(CAP_APPEND, target);
    if (ret != nullptr)
    {
        free(ret->pbmih);
        free(ret);
    }
    return true;
}

bool HRProcessor::Register(HWND hWnd, HRPROCESSCONFIG* pstConfig)
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
    case CAP_APPEND:
        //waiting list is at most ustCapCapacity long
        if (task_waiting_list_.size() == max_task_list_len_)
        {
            // pop the oldest task
            target = task_waiting_list_.front();
            task_waiting_list_.pop_front();
        }
        task_processing_list_.push_back(pstTask);
        break;
    case CAP_PROCESS_OVER:
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
    case CAP_CLEAR_ALL:
        for (it = task_processing_list_.cbegin(); it != task_processing_list_.cend(); ++it)
            free(*it);
        for (it = task_waiting_list_.cbegin(); it != task_waiting_list_.cend(); ++it)
            free(*it);
        break;
    case CAP_GET_FOR_PROCESS:
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
        if (!instance_.AlterstTaskList(CAP_GET_FOR_PROCESS, &stCap))
            // on exiting thread
            break;

        GetCurrentDirectory(128, szCurrentDir);
        //MessageBox(NULL, szCurrentDir, L"szCurrentDir", MB_OK);
        i = -1;
        //find the first file name which is available to create .bmp(in other word, the path does not exist), 
        //as there may be multiple working threads creating files
        WaitForSingleObject(instance_.hMutexCreateFile_, NULL);
        do {
            i++;
            szBitmapFilePath[0] = L'\0';
            wcscpy_s(szBitmapFilePath, szCurrentDir);
            wcscat_s(szBitmapFilePath, L"\\");
            wsprintf(szFileName, L"%d.bmp", i);
            wcscat_s(szBitmapFilePath, szFileName);
            hFindFile = FindFirstFile(szBitmapFilePath, &stFindData);
        } while (hFindFile != INVALID_HANDLE_VALUE);
        //MessageBox(NULL, szBitmapFilePath, L"szBitmapFilePath", MB_OK);
        // TODO
        SavePackedDIBtoFile(szBitmapFilePath, stCap.pbmih);
        ReleaseMutex(instance_.hMutexCreateFile_);

        DeleteDC(stCap.hDC);
        DeleteObject(stCap.hBitmap);

        //output file name
        wcscpy_s(szOutFilePath, szCurrentDir);
        wcscat_s(szOutFilePath, L"\\");
        wsprintf(szFileName, L"%d.png", i);
        wcscat_s(szOutFilePath, szFileName);
        //MessageBox(NULL, szOutFilePath, L"szOutFilePath", MB_OK);

        //output converted file name
        wcscpy_s(szOutConvertedFilePath, szCurrentDir);
        wcscat_s(szOutConvertedFilePath, L"\\");
        wsprintf(szFileName, L"%d_.bmp", i);
        wcscat_s(szOutConvertedFilePath, szFileName);
        //MessageBox(NULL, szOutConvertedFilePath, L"szOutConvertedFilePath", MB_OK);

        GetStartupInfo(&stStartUp);
        stStartUp.wShowWindow = SW_HIDE;
        stStartUp.dwFlags = stStartUp.dwFlags || STARTF_USESHOWWINDOW;

        //construct cmdline for HR exe
        wsprintf(szCmdLine, szCmdLineBase, szHRExePath, szBitmapFilePath, szOutFilePath, uHRScale, szModelrealesrgan_x4plus_anime);
        //MessageBox(NULL, szCmdLine, L"HR", MB_OK);
        ZeroMemory(&stProcInfo, sizeof(stProcInfo));
        if (CreateProcess(szHRExePath, szCmdLine, NULL, NULL, NULL, NORMAL_PRIORITY_CLASS, NULL, szHRExeDir, &stStartUp, &stProcInfo))
        {
            WaitForSingleObject(stProcInfo.hProcess, INFINITE);
            CloseHandle(stProcInfo.hProcess);
            CloseHandle(stProcInfo.hThread);
        }

        //construct cmdline for ffmpeg to extract bmp from png
        GetClientRect(hWndDisplay, &stRect);
        //wsprintf(szCmdLine, szffmpegBase, szffmpegExePath, szOutFilePath, szOutConvertedFilePath);
        wsprintf(szCmdLine, szffmpegResizeBase, szffmpegExePath, szOutFilePath, stRect.right - stRect.left, stRect.bottom - stRect.top, szOutConvertedFilePath);
        //MessageBox(NULL, szCmdLine, L"ffmpeg", MB_OK);
        ZeroMemory(&stProcInfo, sizeof(stProcInfo));
        if (CreateProcess(szffmpegExePath, szCmdLine, NULL, NULL, NULL, NORMAL_PRIORITY_CLASS, NULL, szHRExeDir, &stStartUp, &stProcInfo))
        {
            WaitForSingleObject(stProcInfo.hProcess, INFINITE);
            CloseHandle(stProcInfo.hProcess);
            CloseHandle(stProcInfo.hThread);
        }

        pbmfh = LoadDIBfromFile(szOutConvertedFilePath);
        if (!pbmfh)
        {
            MessageBox(NULL, L"Load Failed!", NULL, MB_OK);
            goto ret;
        }
        pbmi = (BITMAPINFO*)(pbmfh + 1);
        pbmih = (BITMAPINFOHEADER*)pbmi;
        if (pbmih->biSize == sizeof(BITMAPINFOHEADER))
        {
            w = pbmih->biWidth;
            h = abs(pbmih->biHeight);
        }
        else
        {
            pbmch = (BITMAPCOREHEADER*)pbmih;
            w = pbmch->bcWidth;
            h = abs(pbmch->bcHeight);
        }
        //wsprintf(szBuffer, L"%d %d", w, h);
        //MessageBox(NULL, szBuffer, NULL, MB_OK);
        pBits = (BYTE*)pbmfh + pbmfh->bfOffBits;
        hDCDisplay = GetDC(hWndDisplay);
        //GetClientRect(hWndDisplay, &stRect);
        //StretchDIBits(hDCDisplay, 0, 0, stRect.right - stRect.left, stRect.bottom - stRect.top, 0, 0, w, h, pBits, pbmi, DIB_RGB_COLORS, SRCCOPY);
        SetDIBitsToDevice(hDCDisplay, 0, 0, w, h, 0, 0, 0, h, pBits, pbmi, DIB_RGB_COLORS);
        ReleaseDC(hWndDisplay, hDCDisplay);
    ret:
        DeleteFile(szBitmapFilePath);
        DeleteFile(szOutFilePath);
        DeleteFile(szOutConvertedFilePath);
        AlterstTaskList(&stCap, CAP_PROCESS_OVER);
    }
    return NULL;
}
