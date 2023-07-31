@echo off
set cwd=%cd%
set old_path=%PATH%

PATH=%cwd%\lib;%PATH%
@REM start ./bin/win_proj.exe
/usr/x86_64-w64-mingw32/bin/gdb.exe --args ./bin/win_build.exe "/home/koki1019/.vault_projects/Just a game/"

PATH=%old_path%