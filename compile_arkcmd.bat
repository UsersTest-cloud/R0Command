@echo off
call "D:\VS\VC\Auxiliary\Build\vcvars64.bat"
cd /d "d:\Win11Desktop\Project\WDK ARK\arkcmd"
cl.exe /EHsc /W4 /O2 /Fe:arkcmd.exe arkcmd.c /link advapi32.lib
