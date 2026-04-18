/*
* WDK ARK 工具 - IRP派遣函数文件
*
* 本软件仅用于 Windows 内核学习、驱动开发调试、系统安全检测、
* 教学研究、个人设备维护。禁止用于非法入侵、远程控制、恶意
* 攻击、数据窃取、破坏系统、隐藏操作等违法行为。
*/

#include <ntifs.h>
#include <ntddk.h>
#include "protocol.h"

extern PDEVICE_OBJECT g_DeviceObject;

NTSTATUS CompleteIrp(PIRP Irp, NTSTATUS Status, ULONG_PTR Information)
{
    Irp->IoStatus.Status = Status;
    Irp->IoStatus.Information = Information;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return Status;
}

NTSTATUS DispatchCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    return CompleteIrp(Irp, STATUS_SUCCESS, 0);
}

NTSTATUS DispatchClose(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    return CompleteIrp(Irp, STATUS_SUCCESS, 0);
}

NTSTATUS DispatchDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    PIO_STACK_LOCATION stackLoc;
    NTSTATUS status = STATUS_SUCCESS;
    ULONG_PTR information = 0;
    ULONG ioControlCode;
    PVOID inputBuffer;
    PVOID outputBuffer;
    ULONG inputBufferLength;
    ULONG outputBufferLength;

    UNREFERENCED_PARAMETER(DeviceObject);

    stackLoc = IoGetCurrentIrpStackLocation(Irp);
    ioControlCode = stackLoc->Parameters.DeviceIoControl.IoControlCode;
    inputBuffer = Irp->AssociatedIrp.SystemBuffer;
    outputBuffer = Irp->UserBuffer;
    inputBufferLength = stackLoc->Parameters.DeviceIoControl.InputBufferLength;
    outputBufferLength = stackLoc->Parameters.DeviceIoControl.OutputBufferLength;

    if (outputBuffer == NULL || outputBufferLength == 0)
    {
        return CompleteIrp(Irp, STATUS_INVALID_PARAMETER, 0);
    }

    RtlZeroMemory(outputBuffer, outputBufferLength);

    switch (ioControlCode)
    {
    case IOCTL_ENUM_PROCESSES:
    {
        if (outputBufferLength >= sizeof(PROCESS_INFO) * MAX_PROCESS_COUNT)
        {
            PPROCESS_INFO processes = (PPROCESS_INFO)outputBuffer;
            ULONG count = 0;
            status = EnumProcesses(processes, MAX_PROCESS_COUNT, &count);
            if (NT_SUCCESS(status))
            {
                information = count * sizeof(PROCESS_INFO);
            }
        }
        else
        {
            status = STATUS_BUFFER_TOO_SMALL;
        }
        break;
    }

    case IOCTL_GET_PROCESS_DETAIL:
    {
        if (inputBufferLength >= sizeof(ULONG) && outputBufferLength >= sizeof(PROCESS_DETAIL))
        {
            ULONG pid = *(PULONG)inputBuffer;
            PPROCESS_DETAIL detail = (PPROCESS_DETAIL)outputBuffer;
            status = GetProcessDetail(pid, detail);
            if (NT_SUCCESS(status))
            {
                information = sizeof(PROCESS_DETAIL);
            }
        }
        else
        {
            status = STATUS_INVALID_PARAMETER;
        }
        break;
    }

    case IOCTL_ENUM_THREADS:
    {
        if (inputBufferLength >= sizeof(ULONG) && outputBufferLength >= sizeof(THREAD_INFO) * MAX_THREAD_COUNT)
        {
            ULONG pid = *(PULONG)inputBuffer;
            PTHREAD_INFO threads = (PTHREAD_INFO)outputBuffer;
            ULONG count = 0;
            status = EnumThreads(pid, threads, MAX_THREAD_COUNT, &count);
            if (NT_SUCCESS(status))
            {
                information = count * sizeof(THREAD_INFO);
            }
        }
        else
        {
            status = STATUS_INVALID_PARAMETER;
        }
        break;
    }

    case IOCTL_ENUM_MODULES:
    {
        if (outputBufferLength >= sizeof(MODULE_INFO) * MAX_MODULE_COUNT)
        {
            PMODULE_INFO modules = (PMODULE_INFO)outputBuffer;
            ULONG count = 0;
            status = EnumModules(modules, MAX_MODULE_COUNT, &count);
            if (NT_SUCCESS(status))
            {
                information = count * sizeof(MODULE_INFO);
            }
        }
        else
        {
            status = STATUS_BUFFER_TOO_SMALL;
        }
        break;
    }

    case IOCTL_GET_MODULE_DETAIL:
    {
        if (inputBufferLength >= sizeof(WCHAR) * 64 && outputBufferLength >= sizeof(MODULE_DETAIL))
        {
            PWCHAR moduleName = (PWCHAR)inputBuffer;
            PMODULE_DETAIL detail = (PMODULE_DETAIL)outputBuffer;
            status = GetModuleDetail(moduleName, detail);
            if (NT_SUCCESS(status))
            {
                information = sizeof(MODULE_DETAIL);
            }
        }
        else
        {
            status = STATUS_INVALID_PARAMETER;
        }
        break;
    }

    case IOCTL_GET_MODULE_EXPORTS:
    {
        if (inputBufferLength >= sizeof(ULONG_PTR) && outputBufferLength >= sizeof(EXPORT_ENTRY) * MAX_EXPORT_COUNT)
        {
            ULONG_PTR baseAddress = *(PULONG_PTR)inputBuffer;
            PEXPORT_ENTRY exports = (PEXPORT_ENTRY)outputBuffer;
            ULONG count = 0;
            status = GetModuleExports(baseAddress, exports, MAX_EXPORT_COUNT, &count);
            if (NT_SUCCESS(status))
            {
                information = count * sizeof(EXPORT_ENTRY);
            }
        }
        else
        {
            status = STATUS_INVALID_PARAMETER;
        }
        break;
    }

    case IOCTL_GET_SSDT:
    {
        if (outputBufferLength >= sizeof(SSDT_ENTRY) * 512)
        {
            PSSDT_ENTRY entries = (PSSDT_ENTRY)outputBuffer;
            ULONG count = 0;
            status = GetSSDTable(entries, 512, &count);
            if (NT_SUCCESS(status))
            {
                information = count * sizeof(SSDT_ENTRY);
            }
        }
        else
        {
            status = STATUS_BUFFER_TOO_SMALL;
        }
        break;
    }

    case IOCTL_GET_SHADOW_SSDT:
    {
        if (outputBufferLength >= sizeof(SSDT_ENTRY) * 512)
        {
            PSSDT_ENTRY entries = (PSSDT_ENTRY)outputBuffer;
            ULONG count = 0;
            status = GetShadowSSDTable(entries, 512, &count);
            if (NT_SUCCESS(status))
            {
                information = count * sizeof(SSDT_ENTRY);
            }
        }
        else
        {
            status = STATUS_BUFFER_TOO_SMALL;
        }
        break;
    }

    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    return CompleteIrp(Irp, status, information);
}
