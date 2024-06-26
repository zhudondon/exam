#include <napi.h>
#include <windows.h>
#include <ole.h>
#include <winnt.h>
#include "stdio.h"
#include <tlhelp32.h>

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif

//for the getaddrinfo test
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32")

//int GID_BEGIN = 1;
//int GID_END = 2;
//#define GID_ZOOM  3;
//#define GID_PAN  4;
//#define GID_ROTATE  5;
//#define GID_TWOFINGERTAP  6;
//#define GID_PRESSANDTAP 7;
//#define WM_GESTURE 0x0119;

using namespace Napi;
int globlePid = 0;
HHOOK keyHook = NULL;
HHOOK mouseHook = NULL;
HHOOK windowHook = NULL;






//Napi::Buffer<void *> wndHandle = info[0].As<Napi::Buffer<void *>>();
////窗口
//HWND hwndParent = static_cast<HWND>(*reinterpret_cast<void **>(wndHandle.Data()));


//键盘钩子过程
LRESULT CALLBACK keyProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    //在WH_KEYBOARD_LL模式下lParam 是指向KBDLLHOOKSTRUCT类型地址
    KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT *)lParam;
    //如果nCode等于HC_ACTION则处理该消息，如果小于0，则钩子子程就必须将该消息传递给 CallNextHookEx
    //if (nCode == HC_ACTION){
    if (pkbhs->vkCode == VK_ESCAPE && GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState(VK_SHIFT) & 0x8000){
        // qDebug() << "Ctrl+Shift+Esc";
        return 1;
    }
    else if (pkbhs->vkCode == VK_ESCAPE && GetAsyncKeyState(VK_CONTROL) & 0x8000){
        // qDebug() << "Ctrl+Esc";
        return 1;
    }
    else if (pkbhs->flags & LLKHF_ALTDOWN) {
        // qDebug() << "Alt";
        return 1;
    }
    else if (pkbhs->vkCode == LLKHF_ALTDOWN && pkbhs->flags & LLKHF_ALTDOWN) {
        // qDebug() << "Alt+Alt";
        return 1;
    }
    else if (pkbhs->vkCode == VK_TAB && pkbhs->flags & LLKHF_ALTDOWN){
        // qDebug() << "Alt+Tab";
        return 1;
    }
    else if (pkbhs->vkCode == VK_ESCAPE && pkbhs->flags &LLKHF_ALTDOWN){
        // qDebug() << "Alt+Esc";
        return 1;
    }
    else if (pkbhs->vkCode == VK_LWIN || pkbhs->vkCode == VK_RWIN){
        // qDebug() << "LWIN/RWIN";
        return 1;
    }
    else if (pkbhs->vkCode == VK_F4 && pkbhs->flags & LLKHF_ALTDOWN){
        // qDebug() << "Alt+F4";
        return 1;
    }
    else if (pkbhs->vkCode == VK_F11 || pkbhs->flags == VK_F11){
        // qDebug() << "F11";
        return 1;
    } else if (pkbhs->vkCode == VK_ESCAPE){
        // qDebug() << "Esc";
        return 1;
    }

    //return 1;//返回1表示截取消息不再传递,返回0表示不作处理,消息继续传递
    //}
    return CallNextHookEx(keyHook, nCode, wParam, lParam);
}


int decodeGesture(int message, WPARAM wParam, LPARAM lParam){
    // Create a structure to populate and retrieve the extra message info.
    GESTUREINFO gi;

    ZeroMemory(&gi, sizeof(GESTUREINFO));
    gi.cbSize = sizeof(GESTUREINFO);

    BOOL bResult  = GetGestureInfo((HGESTUREINFO)lParam, &gi);
    BOOL bHandled = FALSE;

    if (bResult){
        // now interpret the gesture
        switch (gi.dwID){
            case GID_ZOOM:
                // Code for zooming goes here
//                bHandled = TRUE;
                //可以使用
                bHandled = FALSE;
                break;
            case GID_PAN:
                // Code for panning goes here
                bHandled = TRUE;
                break;
            case GID_ROTATE:
                // Code for rotation goes here
//                bHandled = TRUE;
                //可以使用
                bHandled = FALSE;
                break;
            case GID_TWOFINGERTAP:
                // Code for two-finger tap goes here
//                bHandled = TRUE;
                //可以使用
                bHandled = FALSE;
                break;
            case GID_PRESSANDTAP:
                // Code for roll over goes here
//                bHandled = TRUE;
                //可以使用
                bHandled = FALSE;
                break;

            default:
                // A gesture was not recognized
                // 未知 全部手势控制了 包括3指，4指
                bHandled = TRUE;
                break;
        }
    }else{
        DWORD dwErr = GetLastError();
        if (dwErr > 0){
            //MessageBoxW(hWnd, L"Error!", L"Could not retrieve a GESTUREINFO structure.", MB_OK);
        }
    }
    if (bHandled){
        return 0;
    }else{
        return 1;
       // return DefWindowProc(hwndParent, message, wParam, lParam);
    }
}



LRESULT CALLBACK wndProc(int message, WPARAM wParam, LPARAM lParam) {
//    int wmId, wmEvent;
//    PAINTSTRUCT ps;
//    HDC hdc;

    switch (message) {
        case WM_GESTURE:
            // Insert handler code here to interpret the gesture.
            if(decodeGesture(message, wParam, lParam) == 0){
                //检测到手势 异常手势
                return 1;
            }
    }

}

//鼠标钩子过程
LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
//    if (nCode >= 0)
//    {
//        KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT *)lParam;
//        MSLLHOOKSTRUCT hookStruct = (MSLLHOOKSTRUCT)Marshal.PtrToStructure(lParam, typeof(MSLLHOOKSTRUCT));
//        WM_TOUCH
//        unsigned int extraInfo = (unsigned int) hookStruct.dwExtraInfo.ToInt32();
//        if ((extraInfo & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH)
//        {
//            return 1;
//        }



//    }

    if(nCode == WM_TOUCH){
        unsigned int nNumInputs = (int)wParam;
        // 触摸位置
        if (nNumInputs >= 3)
        {
            return 1;
        }
    }

    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}



//卸载钩子
void unHook()
{
    UnhookWindowsHookEx(keyHook);
    UnhookWindowsHookEx(mouseHook);
    UnhookWindowsHookEx(windowHook);
}

//安装钩子,调用该函数即安装钩子
void setHook()
{
    //这两个底层钩子,不要DLL就可以全局
    //底层键盘钩子
    keyHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyProc, GetModuleHandle(NULL), 0);
    //底层鼠标钩子
    mouseHook =SetWindowsHookEx( WH_MOUSE_LL,mouseProc,GetModuleHandle(NULL),0);

    //手势
    windowHook = SetWindowsHookEx( WH_GETMESSAGE,wndProc,GetModuleHandle(NULL),0);

}


BOOL EnablePriv()
{
    HANDLE hToken;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
        TOKEN_PRIVILEGES tkp;

        LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);//修改进程权限
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL);//通知系统修改进程权限

        return((GetLastError() == ERROR_SUCCESS));
    }
    return TRUE;
}


char* ConvertLPWSTRToLPSTR(LPWSTR lpwszStrIn)
{
    LPSTR pszOut = NULL;
    if (lpwszStrIn != NULL)
    {
        int nInputStrLen = wcslen(lpwszStrIn);

        // Double NULL Termination
        int nOutputStrLen = WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, NULL, 0, 0, 0) + 2;
        pszOut = new char[nOutputStrLen];

        if (pszOut)
        {
            memset(pszOut, 0x00, nOutputStrLen);
            WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, pszOut, nOutputStrLen, 0, 0);
        }
    }
    return pszOut;
}

//冻结
void Freeze()
{
    //枚举进程信息
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    int processPid;
    //CString strTmp;
    BOOL b = ::Process32First(hProcessSnap, &pe32);
    while (b)
    {
        processPid = pe32.th32ProcessID;
        char *exeFile = ConvertLPWSTRToLPSTR(pe32.szExeFile);
        if (strcmp(exeFile, "winlogon.exe") == 0)
        {
            break;
        }

        delete[] exeFile;

        b = ::Process32Next(hProcessSnap, &pe32);
    }
    ::CloseHandle(hProcessSnap);

    THREADENTRY32 th32;
    th32.dwSize = sizeof(th32);
    HANDLE hThreadSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    globlePid = processPid;
    unsigned long Pid;
    Pid = processPid;
    b = ::Thread32First(hThreadSnap, &th32);
    while (b)
    {
        if (th32.th32OwnerProcessID == Pid)
        {
            HANDLE oth = OpenThread(THREAD_ALL_ACCESS, FALSE, th32.th32ThreadID);
            if (!(::SuspendThread(oth)))
            {
                // qDebug() << "freeze successed";
            }
            else
            {
                // qDebug() << "freeze failed";
            }
            CloseHandle(oth);
            break;
        }
        ::Thread32Next(hThreadSnap, &th32);
    }
    ::CloseHandle(hThreadSnap);
}

void unFreeze()
{
    unsigned long Pid;
    Pid = globlePid;

    THREADENTRY32 th32;
    th32.dwSize = sizeof(th32);

    HANDLE hThreadSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    BOOL b = ::Thread32First(hThreadSnap, &th32);
    while (b)
    {
        if (th32.th32OwnerProcessID == Pid)
        {
            HANDLE oth = OpenThread(THREAD_ALL_ACCESS, FALSE, th32.th32ThreadID);
            if (::ResumeThread(oth))
            {
                // qDebug() << "unfreeze successed";
            }
            else
            {
                // qDebug() << "unfreeze failed";
            }
            CloseHandle(oth);
            break;
        }
        ::Thread32Next(hThreadSnap, &th32);
    }
    ::CloseHandle(hThreadSnap);
}





String Lock(const CallbackInfo& info) {
    //屏蔽其他快捷键
    setHook();
    return String::New(info.Env(), "success");
}
String Unlock(const CallbackInfo& info) {
//    unFreeze();
    unHook();
    return String::New(info.Env(), "success");
}
String Hello(const CallbackInfo& info) {
    return String::New(info.Env(), "world");
}


//挂起
String doFreeze(const CallbackInfo& info) {
    //屏蔽其他快捷键
    EnablePriv();
    Freeze();
    return String::New(info.Env(), "success");
}
//唤醒
String doUnFreeze(const CallbackInfo& info) {
    unFreeze();
    return String::New(info.Env(), "success");
}


Napi::Object  Init(Env env, Object exports) {
    exports.Set("hello", Function::New(env, Hello));
    exports.Set("lock", Function::New(env, Lock));
    exports.Set("unlock", Function::New(env, Unlock));
    exports.Set("doFreeze", Function::New(env, doFreeze));
    exports.Set("doUnFreeze", Function::New(env, doUnFreeze));
    return exports;
}

NODE_API_MODULE(addon, Init)