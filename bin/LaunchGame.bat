@echo off
set cwd=%cd%
set old_path=%PATH%

PATH=%cwd%\lib;%PATH%
start ./lib/game.exe
PATH=%old_path%