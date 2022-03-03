// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <list>
#include <map>

using namespace std;

LRESULT WINAPI GetMessageHookProc(int code, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI CallWndProcRetHookProc(int code, WPARAM wParam, LPARAM lParam);


HINSTANCE hInstance;
// an array of lists. elements in every list in this array shares same hHook.
list<HOOKINFO*> lplistHooks[HOOK_TYPE_NUM];
HOOKPROC lplistHookProcs[HOOK_TYPE_NUM] = {
    NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, 
    CallWndProcRetHookProc, NULL, NULL, NULL,
};
int dwCurHookIdx = 0;


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        hInstance = hModule;
        break;
    }
    return TRUE;
}


// Just send message to window which requested hook installation.
LRESULT WINAPI GetMessageHookProc(int code, WPARAM wParam, LPARAM lParam)
{
    list<HOOKINFO*> listHook = lplistHooks[WH_GETMESSAGE];
    HOOKINFO* lpHookInfo;
    if (code < 0)
    {  
        // do not process message 
        lpHookInfo = listHook.front();
        return CallNextHookEx(lpHookInfo->hHook, code, wParam, lParam);
    }
    for (auto it = listHook.cbegin(); it != listHook.cend(); ++it)
    {
        lpHookInfo = *it;
        PostMessage(lpHookInfo->hWnd, lpHookInfo->uMsg, wParam, lParam);
    }
    lpHookInfo = listHook.front();
    return CallNextHookEx(lpHookInfo->hHook, code, wParam, lParam);
}

LRESULT WINAPI CallWndProcRetHookProc(int code, WPARAM wParam, LPARAM lParam)
{
    return TRUE;
}

HOOKINFO* InstallHook(int idcode, DWORD dwThreadId, HWND hWnd, UINT uMsg)
{
    list<HOOKINFO*> listHook;
    HOOKINFO* pstHookInfo = new HOOKINFO;
    int pos;
    if (idcode == WH_MSGFILTER)
        pos = 15;
    else
        pos = idcode;
    listHook = lplistHooks[pos];
    if (listHook.size() == 0)
        pstHookInfo->hHook = SetWindowsHookEx(idcode, GetMessageHookProc, hInstance, dwThreadId);
    else
        pstHookInfo->hHook = listHook.front()->hHook;
    pstHookInfo->hWnd = hWnd;
    pstHookInfo->uMsg = uMsg;
    pstHookInfo->uHookType = WH_CALLWNDPROCRET;
    pstHookInfo->dwThreadId = dwThreadId;
    listHook.push_back(pstHookInfo);
    return pstHookInfo;
}

BOOL UninstallCallWndProcRetHook(HOOKINFO* pstHookInfo)
{
    list<HOOKINFO*> listHook;
    int i;
    for (i = 0; i < HOOK_TYPE_NUM; i++)
    {
        listHook = lplistHooks[i];
        if (!listHook.empty())
            // find out the correct list according to hHook.
            if (listHook.front()->hHook == pstHookInfo->hHook)
            {
                if (listHook.size() == 1)
                {
                    if (UnhookWindowsHookEx(pstHookInfo->hHook))
                    {
                        listHook.pop_back();
                        delete pstHookInfo;
                        return TRUE;
                    }
                    return FALSE;
                }
                for (auto it = listHook.cbegin(); it != listHook.cend(); ++it)
                {
                    if (pstHookInfo == *it)
                    {
                        listHook.erase(it);
                        delete pstHookInfo;
                        return TRUE;
                    }
                }
                return FALSE;
            }
    }
    return FALSE;
}


BOOL Install
