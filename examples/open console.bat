@echo off
if "%1"=="" (
	echo Drag the folder into this file to start
	pause
	exit
)
cd %1
..\..\cpp\neem.exe %~nx1.neem