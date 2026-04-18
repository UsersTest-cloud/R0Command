/*
* WDK ARK 工具 - 驱动入口文件
*
* 本软件仅用于 Windows 内核学习、驱动开发调试、系统安全检测、
* 教学研究、个人设备维护。禁止用于非法入侵、远程控制、恶意
* 攻击、数据窃取、破坏系统、隐藏操作等违法行为。
*/

#include <ntifs.h>
#include <ntddk.h>
#include "protocol.h"

extern NTSTATUS DispatchCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp);
extern NTSTATUS DispatchClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
extern NTSTATUS DispatchDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);

PDEVICE_OBJECT g_DeviceObject = NULL;
PDRIVER_OBJECT g_DriverObject = NULL;
UNICODE_STRING g_DeviceName;
UNICODE_STRING g_SymbolicName;

VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
    PAGED_CODE();

    if (g_SymbolicName.Buffer)
    {
        IoDeleteSymbolicLink(&g_SymbolicName);
        RtlFreeUnicodeString(&g_SymbolicName);
    }

    if (g_DeviceObject)
    {
        IoDeleteDevice(g_DeviceObject);
    }

    DbgPrint("[ARK] Driver unloaded successfully\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    NTSTATUS status;

    UNREFERENCED_PARAMETER(RegistryPath);

    g_DriverObject = DriverObject;

    RtlInitUnicodeString(&g_DeviceName, DEVICE_NAME);
    RtlInitUnicodeString(&g_SymbolicName, SYMBOLIC_NAME);

    status = IoCreateDevice(
        DriverObject,
        0,
        &g_DeviceName,
        FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &g_DeviceObject
    );

    if (!NT_SUCCESS(status))
    {
        DbgPrint("[ARK] Failed to create device: 0x%X\n", status);
        return status;
    }

    status = IoCreateSymbolicLink(&g_SymbolicName, &g_DeviceName);
    if (!NT_SUCCESS(status))
    {
        DbgPrint("[ARK] Failed to create symbolic link: 0x%X\n", status);
        IoDeleteDevice(g_DeviceObject);
        return status;
    }

    DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceControl;
    DriverObject->DriverUnload = DriverUnload;

    g_DeviceObject->Flags |= DO_BUFFERED_IO;
    g_DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    DbgPrint("[ARK] Driver loaded successfully\n");
    DbgPrint("[ARK] Device: %wZ\n", &g_DeviceName);
    DbgPrint("[ARK] Symbolic Link: %wZ\n", &g_SymbolicName);

    return STATUS_SUCCESS;
}
