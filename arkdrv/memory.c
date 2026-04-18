/*
* WDK ARK 工具 - 内存操作文件
*
* 本软件仅用于 Windows 内核学习、驱动开发调试、系统安全检测、
* 教学研究、个人设备维护。禁止用于非法入侵、远程控制、恶意
* 攻击、数据窃取、破坏系统、隐藏操作等违法行为。
*/

#include <ntifs.h>
#include <ntddk.h>
#include "protocol.h"

NTSTATUS ReadVirtualMemory(ULONGLONG Address, PVOID Buffer, ULONG Length)
{
    PVOID mappedAddress = NULL;
    PHYSICAL_ADDRESS physicalAddress = {0};

    __try
    {
        physicalAddress = MmGetPhysicalAddress((PVOID)(ULONG_PTR)Address);
        if (physicalAddress.QuadPart == 0)
        {
            return STATUS_INVALID_ADDRESS;
        }

        mappedAddress = MmMapIoSpace(physicalAddress, Length, MmNonCached);
        if (mappedAddress == NULL)
        {
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        RtlCopyMemory(Buffer, mappedAddress, Length);
        MmUnmapIoSpace(mappedAddress, Length);
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return STATUS_ACCESS_VIOLATION;
    }

    return STATUS_SUCCESS;
}

NTSTATUS ReadPhysicalMemory(ULONGLONG Address, PVOID Buffer, ULONG Length)
{
    PHYSICAL_ADDRESS physicalAddress = {0};
    PVOID mappedAddress = NULL;

    __try
    {
        physicalAddress.QuadPart = Address;

        mappedAddress = MmMapIoSpace(physicalAddress, Length, MmNonCached);
        if (mappedAddress == NULL)
        {
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        RtlCopyMemory(Buffer, mappedAddress, Length);
        MmUnmapIoSpace(mappedAddress, Length);
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return STATUS_ACCESS_VIOLATION;
    }

    return STATUS_SUCCESS;
}

NTSTATUS ReadMemory(ULONG_PTR Address, PVOID Buffer, SIZE_T Size)
{
    return ReadVirtualMemory(Address, Buffer, (ULONG)Size);
}

NTSTATUS WriteMemory(ULONG_PTR Address, PVOID Buffer, SIZE_T Size)
{
    UNREFERENCED_PARAMETER(Address);
    UNREFERENCED_PARAMETER(Buffer);
    UNREFERENCED_PARAMETER(Size);
    return STATUS_NOT_IMPLEMENTED;
}
