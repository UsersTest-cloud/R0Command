/*
* WDK ARK Tool - User Mode Command Console
*/

#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <aclapi.h>

#pragma comment(lib, "advapi32.lib")

#define DEVICE_NAME "\\\\.\\ArkCmd"

#define IOCTL_GET_SYSTEM_BASIC_INFO   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8000, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GET_CPU_INFO            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8001, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ENUM_PROCESSES           CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8100, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ENUM_MODULES            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8200, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ENUM_PROCESS_CALLBACKS  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8300, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ENUM_THREAD_CALLBACKS   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8301, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ENUM_IMAGE_CALLBACKS    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8302, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ENUM_REGISTRY_CALLBACKS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8303, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ENUM_OBJECT_CALLBACKS   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8304, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GET_SSDT                CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8400, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GET_SHADOW_SSDT         CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8401, METHOD_BUFFERED, FILE_ANY_ACCESS)

#pragma pack(push, 1)

typedef struct _SYSTEM_BASIC_INFO {
    WCHAR BuildNumber[64];
    WCHAR KernelType[64];
    WCHAR OsVersion[128];
    ULONG KPCR;
    ULONG KPRCB;
    ULONGLONG CR0;
    ULONGLONG CR3;
    ULONGLONG CR4;
    BOOLEAN PAE;
    BOOLEAN NX;
    BOOLEAN SMEP;
    BOOLEAN SMAP;
    BOOLEAN PatchGuard;
    WCHAR ComputerName[64];
    WCHAR UserName[64];
} SYSTEM_BASIC_INFO, *PSYSTEM_BASIC_INFO;

typedef struct _CPU_INFO {
    WCHAR ProcessorName[128];
    WCHAR Vendor[64];
    ULONG Family;
    ULONG Model;
    ULONG Stepping;
    ULONG Cores;
    ULONG LogicalProcessors;
    ULONGLONG Features;
    ULONGLONG Features2;
} CPU_INFO, *PCPU_INFO;

typedef struct _PROCESS_INFO {
    ULONG_PTR Eprocess;
    ULONG Pid;
    ULONG PPid;
    ULONG SessionId;
    WCHAR Name[256];
    WCHAR ImagePath[MAX_PATH];
    WCHAR CommandLine[512];
} PROCESS_INFO, *PPROCESS_INFO;

typedef struct _MODULE_INFO {
    ULONG_PTR Base;
    ULONG Size;
    WCHAR Name[256];
    WCHAR Path[MAX_PATH];
    WCHAR FileName[256];
    ULONG TimeDateStamp;
} MODULE_INFO, *PMODULE_INFO;

typedef struct _CALLBACK_INFO {
    ULONG_PTR CallbackAddress;
    ULONG_PTR ParentCallback;
    WCHAR ModuleName[256];
    WCHAR FunctionName[128];
    BOOLEAN Hooked;
} CALLBACK_INFO, *PCALLBACK_INFO;

typedef struct _SSDT_ENTRY {
    ULONG Index;
    ULONG_PTR ServiceFunction;
    ULONG_PTR ServiceCount;
    WCHAR ModuleName[64];
    WCHAR FunctionName[128];
    BOOLEAN Hooked;
} SSDT_ENTRY, *PSSDT_ENTRY;

#pragma pack(pop)

HANDLE g_Device = INVALID_HANDLE_VALUE;

BOOL IsAdmin()
{
    BOOL isAdmin = FALSE;
    PSID adminSid = NULL;
    SID_IDENTIFIER_AUTHORITY sia = SECURITY_NT_AUTHORITY;

    if (AllocateAndInitializeSid(&sia, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0, &adminSid))
    {
        CheckTokenMembership(NULL, adminSid, &isAdmin);
        FreeSid(adminSid);
    }

    return isAdmin;
}

void ShowBanner()
{
    printf("\n");
    printf("============================================================\n");
    printf("                    WDK ARK 工具 v1.0                      \n");
    printf("============================================================\n");
    printf("\n");
    printf("仅用于合法用途：Windows 内核学习、驱动调试、系统安全检测。\n");
    printf("\n");
}

void ShowHelp()
{
    printf("\n");
    printf("============================================================\n");
    printf("                        命令帮助                            \n");
    printf("============================================================\n");
    printf("\n");
    printf("  系统信息命令:\n");
    printf("    帮助              - 显示帮助信息\n");
    printf("    清屏              - 清空屏幕\n");
    printf("    版本              - 显示版本信息\n");
    printf("    系统信息          - 显示系统基本信息\n");
    printf("    CPU信息           - 显示 CPU 详细信息\n");
    printf("\n");
    printf("  进程与线程命令:\n");
    printf("    进程列表          - 枚举所有进程\n");
    printf("    模块列表          - 枚举内核模块\n");
    printf("\n");
    printf("  内核回调命令:\n");
    printf("    进程回调          - 显示进程创建回调\n");
    printf("    线程回调          - 显示线程创建回调\n");
    printf("    映像回调          - 显示映像加载回调\n");
    printf("    注册表回调        - 显示注册表回调\n");
    printf("    对象回调          - 显示对象回调\n");
    printf("\n");
    printf("  系统服务表命令:\n");
    printf("    SSDT              - 显示 SSDT 表\n");
    printf("    ShadowSSDT        - 显示 ShadowSSDT 表\n");
    printf("\n");
    printf("  退出:\n");
    printf("    退出              - 退出程序\n");
    printf("\n");
    printf("============================================================\n");
}

void ClearScreen()
{
    system("cls");
}

void ShowVersion()
{
    printf("\n  WDK ARK 工具 v1.0\n");
    printf("  Copyright (C) 2024\n\n");
}

BOOL OpenDevice()
{
    g_Device = CreateFileA(DEVICE_NAME,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (g_Device == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    return TRUE;
}

void CloseDevice()
{
    if (g_Device != INVALID_HANDLE_VALUE)
    {
        CloseHandle(g_Device);
        g_Device = INVALID_HANDLE_VALUE;
    }
}

void ShowSystemInfo()
{
    SYSTEM_BASIC_INFO info = { 0 };
    DWORD bytesReturned = 0;

    if (!DeviceIoControl(g_Device, IOCTL_GET_SYSTEM_BASIC_INFO,
        NULL, 0, &info, sizeof(info), &bytesReturned, NULL))
    {
        printf("  [错误] 获取系统信息失败 (错误码: %d)\n", GetLastError());
        return;
    }

    printf("\n");
    printf("============================================================\n");
    printf("                       系统信息                              \n");
    printf("============================================================\n");
    wprintf(L"  操作系统:         %s\n", info.OsVersion);
    wprintf(L"  版本:             %s Build %s\n", info.KernelType, info.BuildNumber);
    wprintf(L"  计算机名:         %s\n", info.ComputerName);
    wprintf(L"  用户名:           %s\n", info.UserName);
    printf("\n");
    printf("------------------------------------------------------------\n");
    printf("  KPCR:             0x%08X\n", info.KPCR);
    printf("  KPRCB:            0x%08X\n", info.KPRCB);
    printf("  CR0:              0x%016llX\n", info.CR0);
    printf("  CR3:              0x%016llX\n", info.CR3);
    printf("  CR4:              0x%016llX\n", info.CR4);
    printf("\n");
    printf("------------------------------------------------------------\n");
    printf("  PAE:              %s\n", info.PAE ? "已启用" : "已禁用");
    printf("  NX:               %s\n", info.NX ? "已启用" : "已禁用");
    printf("  SMEP:             %s\n", info.SMEP ? "已启用" : "已禁用");
    printf("  SMAP:             %s\n", info.SMAP ? "已启用" : "已禁用");
    printf("  PatchGuard:       %s\n", info.PatchGuard ? "已启用" : "已禁用");
    printf("\n");
}

void ShowCpuInfo()
{
    CPU_INFO info = { 0 };
    DWORD bytesReturned = 0;

    if (!DeviceIoControl(g_Device, IOCTL_GET_CPU_INFO,
        NULL, 0, &info, sizeof(info), &bytesReturned, NULL))
    {
        printf("  [错误] 获取 CPU 信息失败 (错误码: %d)\n", GetLastError());
        return;
    }

    printf("\n");
    printf("============================================================\n");
    printf("                       CPU 信息                              \n");
    printf("============================================================\n");
    wprintf(L"  处理器名称:       %s\n", info.ProcessorName);
    wprintf(L"  供应商:          %s\n", info.Vendor);
    printf("  家族:            %d\n", info.Family);
    printf("  型号:            %d\n", info.Model);
    printf("  步进:            %d\n", info.Stepping);
    printf("  核心数:          %d\n", info.Cores);
    printf("  逻辑处理器:       %d\n", info.LogicalProcessors);
    printf("\n");
    printf("  特性支持:\n");
    printf("    NX:            %s\n", (info.Features & 0x100000) ? "支持" : "不支持");
    printf("    PAE:           %s\n", (info.Features & 0x40000000) ? "支持" : "不支持");
    printf("\n");
}

void ShowProcessList()
{
    PROCESS_INFO processes[256] = { 0 };
    DWORD bytesReturned = 0;

    if (!DeviceIoControl(g_Device, IOCTL_ENUM_PROCESSES,
        NULL, 0, processes, sizeof(processes), &bytesReturned, NULL))
    {
        printf("  [错误] 获取进程列表失败 (错误码: %d)\n", GetLastError());
        return;
    }

    int count = bytesReturned / sizeof(PROCESS_INFO);

    printf("\n");
    printf("============================================================\n");
    printf("                       进程列表                              \n");
    printf("============================================================\n");
    printf("  %-6s %-6s %-6s %-20s\n", "PID", "PPID", "SID", "进程名");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < count; i++)
    {
        wprintf(L"  %-6u %-6u %-6u %-20s\n",
            processes[i].Pid,
            processes[i].PPid,
            processes[i].SessionId,
            processes[i].Name);
    }

    printf("\n  共 %d 个进程\n\n", count);
}

void ShowModuleList()
{
    MODULE_INFO modules[256] = { 0 };
    DWORD bytesReturned = 0;

    if (!DeviceIoControl(g_Device, IOCTL_ENUM_MODULES,
        NULL, 0, modules, sizeof(modules), &bytesReturned, NULL))
    {
        printf("  [错误] 获取模块列表失败 (错误码: %d)\n", GetLastError());
        return;
    }

    int count = bytesReturned / sizeof(MODULE_INFO);

    printf("\n");
    printf("============================================================\n");
    printf("                       模块列表                              \n");
    printf("============================================================\n");
    printf("  %-18s %-10s %-40s\n", "基址", "大小", "名称");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < count; i++)
    {
        wprintf(L"  0x%016llX 0x%08X  %-40s\n",
            modules[i].Base,
            modules[i].Size,
            modules[i].FileName);
    }

    printf("\n  共 %d 个模块\n\n", count);
}

void ShowCallBacks(int type)
{
    CALLBACK_INFO callbacks[64] = { 0 };
    DWORD bytesReturned = 0;
    DWORD ioctl = 0;

    switch (type)
    {
    case 0: ioctl = IOCTL_ENUM_PROCESS_CALLBACKS; break;
    case 1: ioctl = IOCTL_ENUM_THREAD_CALLBACKS; break;
    case 2: ioctl = IOCTL_ENUM_IMAGE_CALLBACKS; break;
    case 3: ioctl = IOCTL_ENUM_REGISTRY_CALLBACKS; break;
    case 4: ioctl = IOCTL_ENUM_OBJECT_CALLBACKS; break;
    default: return;
    }

    if (!DeviceIoControl(g_Device, ioctl,
        NULL, 0, callbacks, sizeof(callbacks), &bytesReturned, NULL))
    {
        printf("  [错误] 获取回调信息失败 (错误码: %d)\n", GetLastError());
        return;
    }

    int count = bytesReturned / sizeof(CALLBACK_INFO);
    const char* titles[] = { "进程创建", "线程创建", "映像加载", "注册表", "对象" };

    printf("\n");
    printf("============================================================\n");
    printf("                     %s 回调                     \n", titles[type]);
    printf("============================================================\n");
    printf("  %-18s %-20s\n", "地址", "模块名");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < count; i++)
    {
        wprintf(L"  0x%016llX %-20s\n",
            callbacks[i].CallbackAddress,
            callbacks[i].ModuleName);
    }

    printf("\n  共 %d 个回调\n\n", count);
}

void ExecuteCommand(const char* command)
{
    if (strcmp(command, "帮助") == 0)
    {
        ShowHelp();
    }
    else if (strcmp(command, "清屏") == 0)
    {
        ClearScreen();
    }
    else if (strcmp(command, "版本") == 0)
    {
        ShowVersion();
    }
    else if (strcmp(command, "系统信息") == 0)
    {
        ShowSystemInfo();
    }
    else if (strcmp(command, "CPU信息") == 0)
    {
        ShowCpuInfo();
    }
    else if (strcmp(command, "进程列表") == 0)
    {
        ShowProcessList();
    }
    else if (strcmp(command, "模块列表") == 0)
    {
        ShowModuleList();
    }
    else if (strcmp(command, "进程回调") == 0)
    {
        ShowCallBacks(0);
    }
    else if (strcmp(command, "线程回调") == 0)
    {
        ShowCallBacks(1);
    }
    else if (strcmp(command, "映像回调") == 0)
    {
        ShowCallBacks(2);
    }
    else if (strcmp(command, "注册表回调") == 0)
    {
        ShowCallBacks(3);
    }
    else if (strcmp(command, "对象回调") == 0)
    {
        ShowCallBacks(4);
    }
    else
    {
        printf("\n  未知命令: %s\n", command);
        printf("  输入 '帮助' 查看可用命令。\n\n");
    }
}

int main(int argc, char* argv[])
{
    ShowBanner();

    if (!IsAdmin())
    {
        printf("  [警告] 未以管理员权限运行！\n");
        printf("  某些功能可能无法正常使用。\n\n");
    }

    if (!OpenDevice())
    {
        printf("  [错误] 无法打开设备 \\Device\\ArkCmd\n");
        printf("  请确保驱动已正确加载。\n");
        printf("  错误代码: %d\n\n", GetLastError());
        printf("  按任意键退出...\n");
        _getch();
        return 1;
    }

    printf("  设备连接成功！\n\n");

    if (argc > 1)
    {
        ExecuteCommand(argv[1]);
    }
    else
    {
        char command[256] = { 0 };

        while (1)
        {
            printf("ARK> ");
            fgets(command, sizeof(command), stdin);

            command[strcspn(command, "\n")] = 0;

            if (strlen(command) == 0)
                continue;

            if (strcmp(command, "退出") == 0)
            {
                printf("再见！\n");
                break;
            }

            ExecuteCommand(command);
        }
    }

    CloseDevice();
    return 0;
}
