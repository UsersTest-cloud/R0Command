#pragma once

#include <wdm.h>
#include <ntddk.h>
#include <ntifs.h>
#include <ntimage.h>

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#define DEVICE_NAME L"\\Device\\ArkDriver"
#define SYMBOLIC_NAME L"\\DosDevices\\ArkDriver"

#define SystemBasicInformation 0
#define SystemCpuInformation 1
#define SystemProcessInformation 5
#define SystemModuleInformation 11

#define IOCTL_ENUM_PROCESSES CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GET_PROCESS_DETAIL CTL_CODE(FILE_DEVICE_UNKNOWN, 0x807, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ENUM_THREADS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x808, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ENUM_HANDLES CTL_CODE(FILE_DEVICE_UNKNOWN, 0x809, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ENUM_MODULES CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80D, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GET_MODULE_DETAIL CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80E, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GET_MODULE_EXPORTS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80F, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GET_SSDT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x813, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GET_SHADOW_SSDT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x814, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define MAX_PROCESS_COUNT 256
#define MAX_THREAD_COUNT 1024
#define MAX_HANDLE_COUNT 4096
#define MAX_MODULE_COUNT 256
#define MAX_EXPORT_COUNT 4096

typedef struct _SYSTEM_BASIC_INFO {
    ULONG Unknown;
    ULONG NumberOfProcessors;
    ULONG ActiveProcessors;
    ULONG64 TimerResolution;
    ULONG64 AllocationGranularity;
    ULONG64 MaxRange;
} SYSTEM_BASIC_INFO, *PSYSTEM_BASIC_INFO;

typedef struct _CPU_INFO {
    UCHAR Architecture;
    UCHAR Level;
    UCHAR Revision;
    UCHAR Brand[64];
    ULONG Features;
    ULONG64 CyclesPerSec;
} CPU_INFO, *PCPU_INFO;

typedef struct _CR_INFO {
    ULONG CrRegister;
    ULONG_PTR Value;
} CR_INFO, *PCR_INFO;

typedef struct _MSR_INFO {
    ULONG MsrRegister;
    ULONG64 Value;
} MSR_INFO, *PMSR_INFO;

typedef struct _GDT_ENTRY {
    USHORT Limit;
    ULONG_PTR Base;
    UCHAR Access;
    UCHAR Flags;
} GDT_ENTRY, *PGDT_ENTRY;

typedef struct _IDT_ENTRY {
    ULONGLONG Offset;
    USHORT Selector;
    UCHAR Type;
    UCHAR Dpl;
    UCHAR Present;
} IDT_ENTRY, *PIDT_ENTRY;

typedef struct _PROCESS_INFO {
    ULONG Pid;
    ULONG PPid;
    WCHAR Name[256];
    ULONG Threads;
    ULONG Handles;
    ULONG SessionId;
    LONG BasePriority;
    ULONG64 VirtualSize;
    ULONG64 WorkingSetSize;
} PROCESS_INFO, *PPROCESS_INFO;

typedef struct _THREAD_INFO {
    ULONG Tid;
    ULONG Pid;
    ULONG ClientId;
    ULONG Priority;
    ULONG State;
    ULONG64 StackBase;
    ULONG64 StackLimit;
    ULONG64 StartAddress;
} THREAD_INFO, *PTHREAD_INFO;

typedef struct _HANDLE_INFO {
    ULONG Handle;
    ULONG Pid;
    ULONG GrantedAccess;
    ULONG ObjectType;
    WCHAR Name[256];
} HANDLE_INFO, *PHANDLE_INFO;

typedef struct _VAD_INFO {
    ULONG_PTR Start;
    ULONG_PTR End;
    ULONG Protect;
    ULONG Type;
    ULONG AllocationProtect;
} VAD_INFO, *PVAD_INFO;

typedef struct _PROCESS_DETAIL {
    ULONG Pid;
    WCHAR Name[256];
    ULONG64 VirtualSize;
    ULONG64 WorkingSetSize;
    ULONG64 PagedPoolUsage;
    ULONG64 NonPagedPoolUsage;
    ULONG64 PagefileUsage;
    ULONG64 PrivatePageCount;
    ULONG64 CommitCharge;
    ULONG_PTR PebBase;
    ULONG_PTR ProcessBase;
    HANDLE Handle;
} PROCESS_DETAIL, *PPROCESS_DETAIL;

typedef struct _MODULE_INFO {
    ULONG_PTR Base;
    ULONG Size;
    WCHAR Name[256];
    WCHAR Path[MAX_PATH];
    WCHAR FileName[256];
    ULONG TimeDateStamp;
} MODULE_INFO, *PMODULE_INFO;

typedef struct _MODULE_DETAIL {
    ULONG_PTR Base;
    ULONG Size;
    WCHAR Name[256];
    WCHAR Path[MAX_PATH];
    ULONG TimeDateStamp;
    ULONG CheckSum;
    USHORT Machine;
    USHORT Characteristics;
} MODULE_DETAIL, *PMODULE_DETAIL;

typedef struct _EXPORT_ENTRY {
    USHORT Ordinal;
    ULONG RVA;
    ULONG Address;
    WCHAR Name[256];
} EXPORT_ENTRY, *PEXPORT_ENTRY;

typedef struct _SSDT_ENTRY {
    ULONG Index;
    ULONG_PTR ServiceFunction;
    ULONG ServiceCount;
    BOOLEAN Hooked;
    WCHAR ModuleName[64];
    WCHAR FunctionName[128];
} SSDT_ENTRY, *PSSDT_ENTRY;

typedef struct _SYSTEM_MODULE_INFORMATION_ENTRY {
    UCHAR Reserved[2];
    UCHAR ImageBase[8];
    ULONG ImageSize;
    ULONG Flags;
    USHORT LoadOrderIndex;
    USHORT InitOrderIndex;
    USHORT ModuleNameOffset;
    CHAR ImagePath[256];
} SYSTEM_MODULE_INFORMATION_ENTRY, *PSYSTEM_MODULE_INFORMATION_ENTRY;

typedef struct _SYSTEM_MODULE_INFORMATION {
    ULONG NumberOfModules;
    SYSTEM_MODULE_INFORMATION_ENTRY Modules[1];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

#pragma pack(push, 1)
typedef struct _KGDTENTRY64 {
    USHORT  LimitLow;
    USHORT  BaseLow;
    union {
        struct {
            UCHAR   BaseMid;
            UCHAR   Flags1;
            UCHAR   BaseHigh;
            UCHAR   Flags2;
        };
        ULONG   BaseHigh32;
    };
    ULONG   BaseUpper;
    ULONG   MustBeZero;
} KGDTENTRY64, *PKGDTENTRY64;

typedef struct _KIDTENTRY64 {
    USHORT  OffsetLow;
    USHORT  Selector;
    USHORT  IstIndex:3;
    USHORT  Reserved:5;
    USHORT  Type:5;
    USHORT  Dpl:2;
    USHORT  Present:1;
    USHORT  OffsetMiddle;
    ULONG   OffsetHigh;
    ULONG   Reserved2;
} KIDTENTRY64, *PKIDTENTRY64;
#pragma pack(pop)

NTSTATUS GetSystemBasicInfo(PSYSTEM_BASIC_INFO Info);
NTSTATUS GetCpuInfo(PCPU_INFO CpuInfo);
NTSTATUS ReadCr(ULONG CrRegister, PULONG_PTR Value);
NTSTATUS ReadMsr(ULONG MsrRegister, PULONGLONG Value);
NTSTATUS GetGdt(PGDT_ENTRY Entries, ULONG MaxCount, PULONG Count);
NTSTATUS GetIdt(PIDT_ENTRY Entries, ULONG MaxCount, PULONG Count);
NTSTATUS EnumProcesses(PPROCESS_INFO Processes, ULONG MaxCount, PULONG Count);
NTSTATUS GetProcessDetail(ULONG Pid, PPROCESS_DETAIL Detail);
NTSTATUS EnumThreads(ULONG Pid, PTHREAD_INFO Threads, ULONG MaxCount, PULONG Count);
NTSTATUS EnumHandles(ULONG Pid, PHANDLE_INFO Handles, ULONG MaxCount, PULONG Count);
NTSTATUS EnumVad(ULONG Pid, PVAD_INFO Vads, ULONG MaxCount, PULONG Count);

NTSTATUS ReadMemory(ULONG_PTR Address, PVOID Buffer, SIZE_T Size);
NTSTATUS WriteMemory(ULONG_PTR Address, PVOID Buffer, SIZE_T Size);

NTSTATUS EnumModules(PMODULE_INFO Modules, ULONG MaxCount, PULONG Count);
NTSTATUS GetModuleDetail(WCHAR* ModuleName, PMODULE_DETAIL Detail);
NTSTATUS GetModuleExports(ULONG_PTR BaseAddress, PEXPORT_ENTRY Exports, ULONG MaxCount, PULONG Count);
NTSTATUS GetSSDTable(PSSDT_ENTRY Entries, ULONG MaxCount, PULONG Count);
NTSTATUS GetShadowSSDTable(PSSDT_ENTRY Entries, ULONG MaxCount, PULONG Count);

NTSYSCALLAPI NTSTATUS NTAPI ZwQuerySystemInformation(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
);
