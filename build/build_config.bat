@echo off

del /f /s /q "../bin/config"
for /d %%i in (../bin/config/*) do rd /s /q "../bin/config/%%i"
excel_export.exe "../config" "../bin/config"
pause