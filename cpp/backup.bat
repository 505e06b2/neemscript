@echo off
set /p COMMIT=Commit: 
if "%COMMIT%"=="" goto :eof
::Need this line since make seems to exit the shell and we also want the variables sandboxes
cmd /c buildjs.bat
make -j5

cd ..
git add *
git commit -am "%COMMIT%"
git push
cd cpp