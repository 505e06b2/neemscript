@echo off
set PATH=..\..\cpp
:a
	set /p filename=Test:
	neem.exe time_%filename%.neem
	goto :a