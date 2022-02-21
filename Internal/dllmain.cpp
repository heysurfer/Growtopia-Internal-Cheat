// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <stdio.h>
#include <iostream>
#include <detours/detours.h>

uintptr_t mBase;
void enableconsole()
{
    AllocConsole();
    freopen("conin$", "r", stdin);
    freopen("conout$", "w", stdout);
    freopen("conout$", "w", stderr);
    printf("Debugging Window:\n");
}

typedef void(__fastcall* _SendConsole)(const char* a1);
_SendConsole SendMsg;


DWORD WINAPI INTERNAL(LPVOID test1, HMODULE test2)
{
    while (!GetAsyncKeyState(VK_END))
    {
        mBase = (uintptr_t)GetModuleHandle(NULL);

        if (GetAsyncKeyState(VK_F1))
        {
            SendMsg("TEST");
        }
        Sleep(100);
    }
    return 0;
}
void SendMsgHooking(const char* a1, ...)
{
    std::cout << "Text :" << a1<<std::endl;
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)SendMsg, SendMsgHooking);
    DetourTransactionCommit();
    std::string test(a1);
    test = "[HeySurfer]" + test;
    SendMsg(test.c_str());
}
BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)

{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        enableconsole();
        mBase = (uintptr_t)GetModuleHandle(NULL);
        DWORD oldprotect;
        VirtualProtectEx(GetCurrentProcess(), (LPVOID)(mBase + 0x3ECBB4), 2, PAGE_EXECUTE_READWRITE, &oldprotect);
        memset((PVOID)(mBase + 0x3ECBB4), 0x74, 1);
        SendMsg = (_SendConsole)((uintptr_t)GetModuleHandle(NULL) + 0x486E10);
    
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)SendMsg, SendMsgHooking);
        DetourTransactionCommit();
        CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(INTERNAL), hMod, 0, nullptr);
        return TRUE;
    }
    }

}

