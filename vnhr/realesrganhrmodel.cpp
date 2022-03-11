#include "realesrganhrmodel.h"

RealesrganHRModel::RealesrganHRModel()
{
    hMutexCreateFile_ = CreateMutex(NULL, FALSE, NULL);
    hr_scale = 4;
}

RealesrganHRModel::~RealesrganHRModel()
{
    CloseHandle(hMutexCreateFile_);
}

BITMAPINFOHEADER* RealesrganHRModel::RunHRAsBitmap(const BITMAPINFOHEADER* pbmihFrom, const RECT* stRectFrom, const RECT* stRectTo)
{
    static const WCHAR szHRExePath[] = L"C:\\Users\\asus\\Downloads\\Compressed\\realesrgan-ncnn-vulkan-20211212-windows\\realesrgan-ncnn-vulkan.exe";
    static const WCHAR szffmpegExePath[] = L"C:\\Users\\asus\\coding\\vsc++\\ffmpeg-4.4.1-full_build-shared\\bin\\ffmpeg.exe";
    static const WCHAR szHRExeDir[] = L"C:\\Users\\asus\\Downloads\\Compressed\\realesrgan-ncnn-vulkan-20211212-windows";
    static const WCHAR szModelrealesrgan_x4plus_anime[] = L"realesrgan-x4plus-anime";
    static const WCHAR szCmdLineBase[] = L"%s -i %s -o %s -s %d -n %s";
    static const WCHAR szffmpegBase[] = L"%s -i %s %s";
    static const WCHAR szffmpegResizeBase[] = L"%s -i %s -s %dx%d %s";
    WCHAR szCurrentDir[128];
    int i;
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
    BITMAPINFOHEADER* pbmihTo;

    GetCurrentDirectory(128, szCurrentDir);
    //MessageBox(NULL, szCurrentDir, L"szCurrentDir", MB_OK);
    i = -1;
    //find the first file name which is available to create .bmp(in other word, the path does not exist), 
    //as there may be multiple working threads creating files
    WaitForSingleObject(hMutexCreateFile_, NULL);
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
    SavePackedDIBtoFile(szBitmapFilePath, pbmihFrom);
    ReleaseMutex(hMutexCreateFile_);

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
    wsprintf(szCmdLine, szCmdLineBase, szHRExePath, szBitmapFilePath, szOutFilePath, hr_scale, szModelrealesrgan_x4plus_anime);
    //MessageBox(NULL, szCmdLine, L"HR", MB_OK);
    ZeroMemory(&stProcInfo, sizeof(stProcInfo));
    if (CreateProcess(szHRExePath, szCmdLine, NULL, NULL, NULL, NORMAL_PRIORITY_CLASS, NULL, szHRExeDir, &stStartUp, &stProcInfo))
    {
        WaitForSingleObject(stProcInfo.hProcess, INFINITE);
    }
    CloseHandle(stProcInfo.hProcess);
    CloseHandle(stProcInfo.hThread);

    //construct cmdline for ffmpeg to extract bmp from png
    //wsprintf(szCmdLine, szffmpegBase, szffmpegExePath, szOutFilePath, szOutConvertedFilePath);
    wsprintf(szCmdLine, szffmpegResizeBase, szffmpegExePath, szOutFilePath, stRectTo->right - stRectTo->left, stRectTo->bottom - stRectTo->top, szOutConvertedFilePath);
    //MessageBox(NULL, szCmdLine, L"ffmpeg", MB_OK);
    ZeroMemory(&stProcInfo, sizeof(stProcInfo));
    if (CreateProcess(szffmpegExePath, szCmdLine, NULL, NULL, NULL, NORMAL_PRIORITY_CLASS, NULL, szHRExeDir, &stStartUp, &stProcInfo))
    {
        WaitForSingleObject(stProcInfo.hProcess, INFINITE);
    }
    CloseHandle(stProcInfo.hProcess);
    CloseHandle(stProcInfo.hThread);

    pbmihTo = LoadPackedDIBfromFile(szOutConvertedFilePath);
    DeleteFile(szBitmapFilePath);
    DeleteFile(szOutFilePath);
    DeleteFile(szOutConvertedFilePath);
    return pbmihTo;
}
