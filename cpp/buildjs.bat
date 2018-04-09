@echo off
if not "%ALREADY%"=="1" call C:\Users\x\Home\emsdk-master\emsdk_env.bat
set ALREADY=1
emmake make -j5 web