@echo off
for /r "." /d %%a in (*) do if /i "%%~nxa"=="RELEASE" rd /s /q %%a
for /r "." /d %%a in (*) do if /i "%%~nxa"=="DEBUG" rd /s /q %%a
for /r "." /d %%a in (*) do if /i "%%~nxa"=="x64" rd /s /q %%a