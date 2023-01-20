#define WIN32_LEAN_AND_MEAN
#pragma warning(disable: 4996)
#include <windows.h>
#include <iostream>

#include "App.h"

App* app;

using namespace std;

/*
#ifdef _WINDLL
bool ErasePE(HMODULE hDLL) {
    uint8_t RandBytes[0x1000];
    srand(GetCurrentThreadId());
    for (auto& i : RandBytes)
        i = (uint8_t)rand();

    return WriteProcessMemory((HANDLE)-1, hDLL, RandBytes, 0x1000, 0);
}

extern "C" __declspec(dllexport)
DWORD WINAPI CustomXboxThread(LPVOID lpParam) {
    app = new App();
    app->Init();
    while (true)
        app->Tick();
    return 0;
}

__declspec(dllexport)
void VmxGetCurrObj()
{

}

__declspec(dllexport)
void VmxTerminateObj()
{

}

__declspec(dllexport)
void VmxCreateObjInfo()
{

}

__declspec(dllexport)
void VmxChangeObjState()
{

}

__declspec(dllexport)
void VmxSetDevice()
{

}

extern "C" __declspec(dllexport)
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD ul_reason_for_call,
    LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        ErasePE(hModule);
        //if (DEBUG)
        {
            AllocConsole();
            freopen("CONOUT$", "w", stdout);
        }
        CreateThread(NULL, NULL, CustomXboxThread, NULL, NULL, NULL);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#else
int main()
{
    app = new App();
    app->Init();
    while (true)
        app->Tick();
}
#endif
*/

int main()
{
    app = new App();
    app->Init();
    while (true)
        app->Tick();
}