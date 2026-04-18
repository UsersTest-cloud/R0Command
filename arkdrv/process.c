/*
* WDK ARK 工具 - 进程线程枚举文件
*
* 本软件仅用于 Windows 内核学习、驱动开发调试、系统安全检测、
* 教学研究、个人设备维护。禁止用于非法入侵、远程控制、恶意
* 攻击、数据窃取、破坏系统、隐藏操作等违法行为。
*/

#include <ntifs.h>
#include <ntddk.h>
#include <ntimage.h>
#include "protocol.h"

NTSTATUS GetSystemBasicInfo(PSYSTEM_BASIC_INFO Info)
{
    UNREFERENCED_PARAMETER(Info);
    return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS GetCpuInfo(PCPU_INFO CpuInfo)
{
    UNREFERENCED_PARAMETER(CpuInfo);
    return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS ReadCr(ULONG CrRegister, PULONG_PTR Value)
{
    UNREFERENCED_PARAMETER(CrRegister);
    UNREFERENCED_PARAMETER(Value);
    return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS ReadMsr(ULONG MsrRegister, PULONGLONG Value)
{
    UNREFERENCED_PARAMETER(MsrRegister);
    UNREFERENCED_PARAMETER(Value);
    return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS GetGdt(PGDT_ENTRY Entries, ULONG MaxCount, PULONG Count)
{
    UNREFERENCED_PARAMETER(Entries);
    UNREFERENCED_PARAMETER(MaxCount);
    *Count = 0;
    return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS GetIdt(PIDT_ENTRY Entries, ULONG MaxCount, PULONG Count)
{
    UNREFERENCED_PARAMETER(Entries);
    UNREFERENCED_PARAMETER(MaxCount);
    *Count = 0;
    return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS EnumProcesses(PPROCESS_INFO Processes, ULONG MaxCount, PULONG Count)
{
    *Count = 0;

    __try
    {
        ULONG bufferSize = 16 * 1024 * 1024;
        PVOID buffer = ExAllocatePool2(PagedPool, bufferSize, 'ARK');

        if (buffer == NULL)
            return STATUS_INSUFFICIENT_RESOURCES;

        NTSTATUS status = ZwQuerySystemInformation(
            SystemProcessInformation,
            buffer,
            bufferSize,
            &bufferSize);

        if (!NT_SUCCESS(status))
        {
            ExFreePool(buffer);
            return status;
        }

        PVOID pCurrent = buffer;
        ULONG offset = 0;
        ULONG resultCount = 0;

        while (offset < bufferSize && resultCount < MaxCount)
        {
            ULONG nextOffset = *(PULONG)pCurrent;
            ULONG threadCount = *(PULONG)((UCHAR*)pCurrent + 4);

            HANDLE pid = *(HANDLE*)((UCHAR*)pCurrent + 80);
            HANDLE ppid = *(HANDLE*)((UCHAR*)pCurrent + 88);

            Processes[resultCount].Pid = (ULONG)(ULONG_PTR)pid;
            Processes[resultCount].PPid = (ULONG)(ULONG_PTR)ppid;
            Processes[resultCount].Threads = threadCount;
            Processes[resultCount].Handles = 0;
            Processes[resultCount].SessionId = 0;
            Processes[resultCount].BasePriority = 0;
            Processes[resultCount].VirtualSize = 0;
            Processes[resultCount].WorkingSetSize = 0;

            USHORT nameLen = *(PUSHORT)((UCHAR*)pCurrent + 72);
            PWCHAR nameBuf = (PWCHAR)((UCHAR*)pCurrent + 80);

            if (nameLen > 0 && nameLen < 512)
            {
                ULONG copyLen = nameLen / 2;
                if (copyLen > 255) copyLen = 255;

                for (ULONG i = 0; i < copyLen; i++)
                {
                    Processes[resultCount].Name[i] = nameBuf[i];
                }
                Processes[resultCount].Name[copyLen] = 0;
            }
            else
            {
                Processes[resultCount].Name[0] = 0;
            }

            resultCount++;

            if (nextOffset == 0)
                break;

            offset += nextOffset;
            pCurrent = (PVOID)((UCHAR*)pCurrent + nextOffset);
        }

        ExFreePool(buffer);
        *Count = resultCount;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return STATUS_ACCESS_VIOLATION;
    }

    return STATUS_SUCCESS;
}

NTSTATUS GetProcessDetail(ULONG Pid, PPROCESS_DETAIL Detail)
{
    UNREFERENCED_PARAMETER(Pid);
    UNREFERENCED_PARAMETER(Detail);
    return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS EnumThreads(ULONG Pid, PTHREAD_INFO Threads, ULONG MaxCount, PULONG Count)
{
    UNREFERENCED_PARAMETER(Pid);
    UNREFERENCED_PARAMETER(Threads);
    UNREFERENCED_PARAMETER(MaxCount);
    *Count = 0;
    return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS EnumHandles(ULONG Pid, PHANDLE_INFO Handles, ULONG MaxCount, PULONG Count)
{
    UNREFERENCED_PARAMETER(Pid);
    UNREFERENCED_PARAMETER(Handles);
    UNREFERENCED_PARAMETER(MaxCount);
    *Count = 0;
    return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS EnumVad(ULONG Pid, PVAD_INFO Vads, ULONG MaxCount, PULONG Count)
{
    UNREFERENCED_PARAMETER(Pid);
    UNREFERENCED_PARAMETER(Vads);
    UNREFERENCED_PARAMETER(MaxCount);
    *Count = 0;
    return STATUS_NOT_IMPLEMENTED;
}
