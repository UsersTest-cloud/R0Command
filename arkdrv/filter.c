/*
* WDK ARK 工具 - 过滤器驱动枚举文件
*
* 本软件仅用于 Windows 内核学习、驱动开发调试、系统安全检测、
* 教学研究、个人设备维护。禁止用于非法入侵、远程控制、恶意
* 攻击、数据窃取、破坏系统、隐藏操作等违法行为。
*/

#include <ntifs.h>
#include <ntddk.h>
#include "protocol.h"

NTSTATUS EnumKernelFilters(PVOID Filters, ULONG MaxCount, PULONG Count)
{
    UNREFERENCED_PARAMETER(Filters);
    UNREFERENCED_PARAMETER(MaxCount);
    *Count = 0;
    return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS EnumWfpFilters(PVOID WfpInfo, ULONG MaxCount, PULONG Count)
{
    UNREFERENCED_PARAMETER(WfpInfo);
    UNREFERENCED_PARAMETER(MaxCount);
    *Count = 0;
    return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS GetFilterRegistry(PVOID Config, ULONG MaxCount, PULONG Count)
{
    UNREFERENCED_PARAMETER(Config);
    UNREFERENCED_PARAMETER(MaxCount);
    *Count = 0;
    return STATUS_NOT_IMPLEMENTED;
}
