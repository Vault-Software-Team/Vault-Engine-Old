@echo off

SET PLATFORM=%1
IF "%PLATFORM%" == "windows" (
    echo Bundling Windows...
    COPY  "assets" "dist\windows\assets"
    COPY  "imgui.ini" "dist\windows\imgui.ini"
    COPY  "shaders" "dist\windows\shaders"
    COPY  "build" "dist\windows\build"
    COPY  "cs-assembly" "dist\windows\cs-assembly"
    COPY  "windows\lib" "dist\windows\lib"
    COPY  "mono\lib" "dist\windows\lib\mono"
    COPY  "windows\bin" "dist\windows\bin"
    COPY  "windows/Vault Engine.bat" "dist/windows/Vault Engine.bat"
) ELSE (
    echo Bundling Linux...
    COPY  "lib" "dist\linux\lib"
    COPY  "bin\build.out" "dist\linux\bin\build.out"
    COPY  "bin\build_proj.out" "dist\linux\bin\build_proj.out"
    COPY  "Vault Engine.sh" "dist/linux/Vault Engine.sh"
    COPY  "assets" "dist\linux\assets"
    COPY  "imgui.ini" "dist\linux\imgui.ini"
    COPY  "shaders" "dist\linux\shaders"
    COPY  "build" "dist\linux\build"
    COPY  "mono" "dist\linux\mono"
    COPY  "cs-assembly" "dist\linux\cs-assembly"
)