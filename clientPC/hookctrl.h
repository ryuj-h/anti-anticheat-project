#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>

using namespace std;


#define protocol_init 0x37903
#define protocol_id 0x37904
#define protocol_base 0x37905
#define protocol_read 0x37906
#define protocol_spoof 0x37907
#define protocol_getmouse 0x37908
#define protocol_setmouse 0x37909
#define protocol_mouseevent 0x37910
#define protocol_getkeystate 0x37911
#define protocol_keybdevent 0x37912
#define protocol_write 0x37913


typedef struct _BUFFER {
    char buffer[0x4000];
}BUFFER, * PBUFFER;

typedef struct _KERNEL_ID
{
    char processname[15];
    ULONG PID;
    ULONG NUM;
}KERNEL_ID, * PKERNEL_ID;

typedef struct _KERNEL_READ
{
    ULONG ProcessId;
    DWORD64 Address;
    BUFFER Response;
    ULONG Size;

} KERNEL_READ, * PKERNEL_READ;

typedef struct _KERNEL_BASE
{
    ULONG ProcessId;
    DWORD64 Address;
} KERNEL_BASE, * PKERNEL_BASE;

typedef struct _KERNEL_HWND_SPOOF
{
    DWORD64 HWND;
    DWORD64 RESULT;
} KERNEL_HWND_SPOOF, * PKERNEL_HWND_SPOOF;

typedef struct _KERNEL_GETSET_MOUSE
{
    ULONG x;
    ULONG y;
} KERNEL_GETSET_MOUSE, * PKERNEL_GETSET_MOUSE;


typedef struct _KERNEL_MOUSEEVENT
{
    ULONG x;
    ULONG y;
    unsigned short button_flags;
} KERNEL_MOUSEEVENT, * PKERNEL_MOUSEEVENT;

typedef struct _KERNEL_GETKEYSTATE
{
    int key;
    short result;
}KERNEL_GETKEYSTATE, * PKERNEL_GETKEYSTATE;

typedef struct _KERNEL_KEYBOARDEVENT
{
    USHORT UnitId;
    USHORT MakeCode;
    USHORT Flags;
    USHORT Reserved;
    ULONG ExtraInformation;
}KERNEL_KEYBOARDEVENT, * PKERNEL_KEYBOARDEVENT;

typedef struct _KERNEL_WRITE {
    ULONG fromPID;
    ULONGLONG Dest;

    ULONG ProcessId;
    ULONGLONG Src;
    ULONG Size;
} KERNEL_WRITE, * PKERNEREL_WRITE;

typedef struct _KERNEL_INIT
{
    ULONG result;
    int off_IMAGE_FILE_NAME;
    int off_ACTIVE_PROCESS_LINKS_FLINK;
    char cwtpPattern[100];
    char cwtpxPattern[100];
    int offsetcwtp;
} KERNEL_INIT, * PKERNEL_INIT;

typedef struct _KERNEL_PROTOCOL
{
    ULONG               calltype;
    KERNEL_ID           kernelID;
    KERNEL_READ         kernelRead;
    KERNEL_BASE         kernelBase;
    KERNEL_HWND_SPOOF   kernelSpoof;
    KERNEL_INIT         kernelInit;
    KERNEL_GETSET_MOUSE kenelmouse;
    KERNEL_MOUSEEVENT   kernelmouseevent;
    KERNEL_GETKEYSTATE  kernelkeystate;
    KERNEL_KEYBOARDEVENT kernelkeybdevent;
    KERNEL_WRITE		kernelWrite;
}KERNEL_PROTOCOL, * PKERNEL_PROTOCOL;

class hkdrv
{
public:
    KERNEL_PROTOCOL protocol;
    PVOID(__fastcall* NtUserCreateDesktopEx)(PVOID a1, PVOID a2, PVOID a3, unsigned int a4, int a5, int a6);
    hkdrv()
    {
        HMODULE win32u = LoadLibraryW(L"win32u.dll");

        *(PVOID*)&NtUserCreateDesktopEx = GetProcAddress(
            GetModuleHandleW(L"win32u.dll"),
            "NtUserCreateDesktopEx"
        );
        if (!init())
        {
            MessageBoxW(0, L"Drv Not loaded", L"ERROR", MB_OK);
            exit(1);
        }
    }
    void SendRequest(int calltype)
    {
        protocol.calltype = calltype;
        NtUserCreateDesktopEx(&protocol, NULL, (PVOID)0x99, NULL, NULL, NULL);
    }

    bool init()
    {
        protocol.kernelInit.result = 0;
        SendRequest(protocol_init);
        if (protocol.kernelInit.result == 0x6974892)
            return true;
        return false;
    }

    void SetCursorPos(int x, int y)
    {
        protocol.kenelmouse.x = x;
        protocol.kenelmouse.y = y;
        SendRequest(protocol_setmouse);
    }

    void GetCursorPos(POINT* point)
    {
        SendRequest(protocol_getmouse);
        point->x = protocol.kenelmouse.x;
        point->y = protocol.kenelmouse.y;
    }

    uint64_t GetPid(char* processname, int num)
    {
        protocol.kernelID.PID = 0;
        strcpy(protocol.kernelID.processname, processname);
        protocol.kernelID.NUM = num;
        SendRequest(protocol_id);
        return protocol.kernelID.PID;
    }

    uint64_t GetBaseAddress(int pid)
    {
        protocol.kernelBase.Address = 0;
        protocol.kernelBase.ProcessId = pid;
        SendRequest(protocol_base);
        return  protocol.kernelBase.Address;
    }

    void RPM(DWORD pid, PVOID dest, DWORD64 source, size_t size)
    {
        protocol.kernelRead.ProcessId = pid;
        protocol.kernelRead.Address = source;
        protocol.kernelRead.Size = size;
        SendRequest(protocol_read);

        memcpy(dest, &protocol.kernelRead.Response, size);
    }
    void WPM(DWORD targetpid, ULONGLONG dest, ULONGLONG src, size_t size)
    {
        protocol.kernelWrite.fromPID = GetCurrentProcessId();
        protocol.kernelWrite.Dest = dest;
        protocol.kernelWrite.ProcessId = targetpid;
        protocol.kernelWrite.Src = src;
        protocol.kernelWrite.Size = size;
        SendRequest(protocol_write);
    }

    void spoofhwnd(HWND hwnd, uint64_t set)
    {
        protocol.kernelSpoof.HWND = (uint64_t)(hwnd);
        protocol.kernelSpoof.RESULT = set;
        SendRequest(protocol_spoof);
    }

    void InitOffset(int off_IMAGE_FILE_NAME, int off_ACTIVE_PROCESS_LINKS_FLINK, char* cwtpPattern, char* cwtpxPattern, int offsetcwtp)
    {
        protocol.kernelInit.off_IMAGE_FILE_NAME = off_IMAGE_FILE_NAME;
        protocol.kernelInit.off_ACTIVE_PROCESS_LINKS_FLINK = off_ACTIVE_PROCESS_LINKS_FLINK;
        protocol.kernelInit.offsetcwtp = offsetcwtp;

        strncpy(protocol.kernelInit.cwtpPattern, cwtpPattern, strlen(cwtpPattern));
        strncpy(protocol.kernelInit.cwtpxPattern, cwtpxPattern, strlen(cwtpxPattern));

        SendRequest(protocol_init);
    }
    void mouse_event(int x, int y, unsigned short flag)
    {
        protocol.kernelmouseevent.x = x;
        protocol.kernelmouseevent.y = y;
        protocol.kernelmouseevent.button_flags = flag;
        SendRequest(protocol_mouseevent);
    }
    bool GetKeystate(int key)
    {
        protocol.kernelkeystate.key = key;
        protocol.kernelkeystate.result = 0;

        SendRequest(protocol_getkeystate);
        return protocol.kernelkeystate.result;
    }

    void keyboardevent(USHORT unitId, USHORT makecode,
        USHORT flags, USHORT reserved,
        ULONG extraInformation)
    {
        protocol.kernelkeybdevent.ExtraInformation = extraInformation;
        protocol.kernelkeybdevent.Flags = flags;
        protocol.kernelkeybdevent.MakeCode = makecode;
        protocol.kernelkeybdevent.Reserved = reserved;
        protocol.kernelkeybdevent.UnitId = unitId;
        SendRequest(protocol_keybdevent);
    }

    UINT PressKey(WORD scanCode)
    {
        keyboardevent(0, scanCode, 0, 0, 0);

        return 0;
    }

    UINT ReleaseKey(WORD scanCode)
    {
        keyboardevent(0, scanCode, 1, 0, 0);

        return 0;
    }
};