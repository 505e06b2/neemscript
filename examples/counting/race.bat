@echo off
set i=0
:a
	set /a "i+=1"
	if "%i%"=="1000000" goto :end
	goto :a
	
:end