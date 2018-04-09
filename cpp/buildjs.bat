@echo off
if "%ALREADY%"=="" call C:\Users\x\Home\emsdk-master\emsdk_env.bat
set ALREADY=finished
emmake make -j5 web
echo gg