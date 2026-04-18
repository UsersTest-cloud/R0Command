@echo off
REM Manual Driver Build Script

call "D:\VS\VC\Auxiliary\Build\vcvars64.bat"

set WDKROOT=C:\Program Files (x86)\Windows Kits\10
set WDKVERSION=10.0.26100.0

set "SRC_DIR=d:\Win11Desktop\Project\WDK ARK\arkdrv"
set "BIN_DIR=d:\Win11Desktop\Project\WDK ARK\bin"
set "OBJ_DIR=d:\Win11Desktop\Project\WDK ARK\obj"

set "WDKINC=%WDKROOT%\include\%WDKVERSION%"
set "WDKLIB=%WDKROOT%\lib\%WDKVERSION%"

if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"
if not exist "%OBJ_DIR%" mkdir "%OBJ_DIR%"

echo [INFO] Building ARK Driver...

set C_FLAGS=/c /W4 /GS- /D_WDKDRIVER_ /DWIN7_ /D_AMD64_ /DAMD64 /D_UNICODE /DUNICODE /I"%WDKINC%\km" /I"%WDKINC%\shared"

set SOURCES=arkdrv.c dispatch.c process.c memory.c callback.c module.c object.c hook.c filter.c

echo [INFO] Compiling source files...

for %%F in (%SOURCES%) do (
    echo     cl %%F
    cl.exe %C_FLAGS% "%SRC_DIR%\%%F" /Fo"%OBJ_DIR%\%%~nF.obj"
    if errorlevel 1 (
        echo [ERROR] Failed to compile %%F
        exit /b 1
    )
)

echo [INFO] Linking driver...

link.exe /DRIVER /FIXED:NO /NODEFAULTLIB /IGNORE:4343 /INCREMENTAL:NO /MACHINE:X64 /ENTRY:DriverEntry /SUBSYSTEM:NATIVE /OUT:"%BIN_DIR%\arkdrv.sys" /MAP:"%BIN_DIR%\arkdrv.map" "%OBJ_DIR%\*.obj" "%WDKLIB%\km\x64\ntoskrnl.lib" "%WDKLIB%\km\x64\hal.lib" "%WDKLIB%\km\x64\wmilib.lib"
if errorlevel 1 (
    echo [ERROR] Failed to link driver
    exit /b 1
)

echo [OK] Driver built successfully!
echo     Output: %BIN_DIR%\arkdrv.sys
echo     Map:    %BIN_DIR%\arkdrv.map
exit /b 0
