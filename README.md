# R0Command

Windows 内核高级分析工具 - Ring0 内核驱动 + Ring3 用户态控制台

**GitHub**: https://github.com/UsersTest-cloud/R0Command

## 项目说明

R0Command 是一个 Windows 内核高级分析工具，包含 Ring0 内核驱动和 Ring3 用户态控制台程序，用于系统安全研究和内核学习。

## 目录结构

```
R0Command/
├── arkdrv/                    # 内核驱动目录
│   ├── arkdrv.c              # 驱动入口
│   ├── arkdrv.sys           # 编译后的驱动
│   ├── dispatch.c           # IRP派遣函数
│   ├── process.c            # 进程/线程操作
│   ├── memory.c             # 内存操作
│   ├── callback.c           # 内核回调枚举
│   ├── module.c            # 内核模块枚举
│   ├── object.c            # 内核对象操作
│   ├── hook.c              # 钩子检测
│   ├── filter.c            # 过滤驱动枚举
│   └── protocol.h           # 通信协议头文件
│
├── arkcmd/                   # 用户态程序目录
│   ├── arkcmd.c             # 命令行控制台
│   └── arkcmd.exe           # 编译后的控制台
│
├── compile_arkcmd.bat        # 用户态程序编译脚本
└── README.md
```

## 构建说明

### 前置条件

1. Windows Driver Kit (WDK) 10.0.26100.0+
2. Visual Studio 2022
3. Windows SDK

### 编译驱动

1. 以管理员权限打开 Visual Studio Developer Command Prompt
2. 运行 `build_driver.bat`

### 编译用户态程序

1. 打开 VS Developer Command Prompt
2. 运行 `compile_arkcmd.bat`

### 驱动安装

1. 将 `arkdrv.sys` 复制到系统目录
2. 以管理员权限运行 `arkcmd.exe`

## 功能列表

- 系统信息查询 (版本、Build号、内核类型)
- CPU 信息查询
- CR0/CR3/CR4 寄存器读取
- GDT/IDT 表枚举
- MSR 寄存器读取
- 进程/线程枚举
- 内核模块枚举
- 模块导出表枚举
- SSDT/ShadowSSDT 枚举
- 内存读取 (虚拟/物理)
- 内核回调枚举
- 对象目录枚举
- 钩子检测
- 过滤驱动枚举

## 合法用途声明

本软件仅用于：
- Windows 内核学习
- 驱动开发调试
- 系统安全检测
- 教学研究
- 个人设备维护

**禁止用于**：
- 非法入侵
- 远程控制
- 恶意攻击
- 数据窃取
- 破坏系统
- 隐藏操作

## 作者

R0Command Development Team

## 版本

v1.0.0
