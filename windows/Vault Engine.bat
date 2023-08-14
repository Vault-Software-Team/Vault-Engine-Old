@echo off
set cwd=%cd%
set old_path=%PATH%

PATH=%cwd%\lib;%PATH%
start ./bin/win_proj.exe

PATH=%old_path%