@echo off
set /p COMMIT=Commit: 
if "%COMMIT%"=="" goto :eof
cmd /c buildjs.bat

cd ..
git add *
git commit -am "%COMMIT%"
git push
cd cpp