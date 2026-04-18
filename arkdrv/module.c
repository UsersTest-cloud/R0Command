/*
* WDK ARK 工具 - 内核模块枚举文件
*
* 本软件仅用于 Windows 内核学习、驱动开发调试、系统安全检测、
* 教学研究、个人设备维护。禁止用于非法入侵、远程控制、恶意
* 攻击、数据窃取、破坏系统、隐藏操作等违法行为。
*/

#include <ntifs.h>
#include <ntddk.h>
#include <ntimage.h>
#include "protocol.h"

NTSTATUS EnumModules(PMODULE_INFO Modules, ULONG MaxCount, PULONG Count)
{
    *Count = 0;

    __try
    {
        ULONG bufferSize = 0;
        NTSTATUS status = ZwQuerySystemInformation(
            SystemModuleInformation,
            NULL,
            0,
            &bufferSize);

        if (status != STATUS_INFO_LENGTH_MISMATCH && status != STATUS_SUCCESS)
            return status;

        if (bufferSize == 0)
            bufferSize = 256 * 1024;

        PVOID buffer = ExAllocatePool2(PagedPool, bufferSize, 'ARK');
        if (buffer == NULL)
            return STATUS_INSUFFICIENT_RESOURCES;

        status = ZwQuerySystemInformation(
            SystemModuleInformation,
            buffer,
            bufferSize,
            &bufferSize);

        if (!NT_SUCCESS(status))
        {
            ExFreePool(buffer);
            return status;
        }

        PSYSTEM_MODULE_INFORMATION moduleInfo = (PSYSTEM_MODULE_INFORMATION)buffer;
        ULONG moduleCount = moduleInfo->NumberOfModules;

        for (ULONG i = 0; i < moduleCount && *Count < MaxCount; i++)
        {
            PSYSTEM_MODULE_INFORMATION_ENTRY entry = &moduleInfo->Modules[i];

            ULONGLONG baseAddr = 0;
            for (int j = 0; j < 8; j++)
            {
                baseAddr |= ((ULONGLONG)entry->ImageBase[j]) << (j * 8);
            }

            Modules[*Count].Base = (ULONG_PTR)baseAddr;
            Modules[*Count].Size = entry->ImageSize;

            ULONG nameOffset = entry->ModuleNameOffset;
            CHAR* fullPath = entry->ImagePath;
            CHAR* fileName = fullPath;

            for (CHAR* p = fullPath; *p != 0; p++)
            {
                if (*p == '\\' || *p == '/')
                    fileName = p + 1;
            }

            ULONG pathLen = 0;
            for (CHAR* p = fullPath; *p != 0 && pathLen < 255; p++)
            {
                Modules[*Count].Path[pathLen] = (WCHAR)*p;
                pathLen++;
            }
            Modules[*Count].Path[pathLen] = 0;

            ULONG nameLen = 0;
            for (CHAR* p = fileName; *p != 0 && nameLen < 255; p++)
            {
                Modules[*Count].Name[nameLen] = (WCHAR)*p;
                Modules[*Count].FileName[nameLen] = (WCHAR)*p;
                nameLen++;
            }
            Modules[*Count].Name[nameLen] = 0;
            Modules[*Count].FileName[nameLen] = 0;

            Modules[*Count].TimeDateStamp = 0;

            (*Count)++;
        }

        ExFreePool(buffer);
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return STATUS_ACCESS_VIOLATION;
    }

    return STATUS_SUCCESS;
}

NTSTATUS GetModuleDetail(WCHAR* ModuleName, PMODULE_DETAIL Detail)
{
    MODULE_INFO modules[256];
    ULONG count = 0;

    NTSTATUS status = EnumModules(modules, 256, &count);
    if (!NT_SUCCESS(status))
        return status;

    for (ULONG i = 0; i < count; i++)
    {
        if (_wcsicmp(modules[i].Name, ModuleName) == 0 ||
            _wcsicmp(modules[i].FileName, ModuleName) == 0)
        {
            RtlZeroMemory(Detail, sizeof(MODULE_DETAIL));

            Detail->Base = modules[i].Base;
            Detail->Size = modules[i].Size;
            wcscpy_s(Detail->Name, 256, modules[i].Name);
            wcscpy_s(Detail->Path, MAX_PATH, modules[i].Path);
            Detail->TimeDateStamp = modules[i].TimeDateStamp;
            Detail->CheckSum = 0;
            Detail->Machine = 0x8664;
            Detail->Characteristics = 0x102;
            return STATUS_SUCCESS;
        }
    }

    return STATUS_NOT_FOUND;
}

NTSTATUS GetModuleExports(ULONG_PTR BaseAddress, PEXPORT_ENTRY Exports, ULONG MaxCount, PULONG Count)
{
    *Count = 0;

    __try
    {
        PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)BaseAddress;
        if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        {
            return STATUS_INVALID_IMAGE_FORMAT;
        }

        PIMAGE_NT_HEADERS64 ntHeader = (PIMAGE_NT_HEADERS64)((ULONG_PTR)dosHeader + dosHeader->e_lfanew);
        if (ntHeader->Signature != IMAGE_NT_SIGNATURE)
        {
            return STATUS_INVALID_IMAGE_FORMAT;
        }

        ULONG exportRVA = ntHeader->OptionalHeader.DataDirectory[0].VirtualAddress;
        if (exportRVA == 0)
            return STATUS_SUCCESS;

        PIMAGE_EXPORT_DIRECTORY exportDir = (PIMAGE_EXPORT_DIRECTORY)(BaseAddress + exportRVA);

        PULONG nameRVAs = (PULONG)(BaseAddress + exportDir->AddressOfNames);
        PUSHORT ordinals = (PUSHORT)(BaseAddress + exportDir->AddressOfNameOrdinals);
        PULONG addresses = (PULONG)(BaseAddress + exportDir->AddressOfFunctions);

        for (ULONG i = 0; i < exportDir->NumberOfNames && *Count < MaxCount; i++)
        {
            PCHAR name = (PCHAR)(BaseAddress + nameRVAs[i]);
            USHORT ordinal = ordinals[i];

            Exports[*Count].Ordinal = ordinal;
            Exports[*Count].RVA = addresses[ordinal];
            Exports[*Count].Address = (ULONG)(BaseAddress + addresses[ordinal]);

            ULONG len = strlen(name);
            for (ULONG j = 0; j < len && j < 255; j++)
            {
                Exports[*Count].Name[j] = name[j];
                Exports[*Count].Name[j + 1] = 0;
            }

            (*Count)++;
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return STATUS_ACCESS_VIOLATION;
    }

    return STATUS_SUCCESS;
}

NTSTATUS GetSSDTable(PSSDT_ENTRY Entries, ULONG MaxCount, PULONG Count)
{
    UNREFERENCED_PARAMETER(Entries);
    UNREFERENCED_PARAMETER(MaxCount);
    *Count = 0;
    return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS GetShadowSSDTable(PSSDT_ENTRY Entries, ULONG MaxCount, PULONG Count)
{
    UNREFERENCED_PARAMETER(Entries);
    UNREFERENCED_PARAMETER(MaxCount);
    *Count = 0;
    return STATUS_NOT_IMPLEMENTED;
}
