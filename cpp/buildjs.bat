@echo off
if "%ALREADY%"=="" call C:\Users\x\Home\emsdk-master\emsdk_env.bat
set ALREADY=finished
emmake make -j5 web OPTIMISATIONS=-O3 LDFLAGS="-s ASSERTIONS=1 -s EXPORTED_FUNCTIONS='[""_webmain""]' -s EXTRA_EXPORTED_RUNTIME_METHODS='[""cwrap""]'"