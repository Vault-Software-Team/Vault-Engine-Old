PLATFORM=$1

if [ "$PLATFORM" = "windows" ]; then
    # check if dist/windows exists
    # if not, create it
    
    if [ -d dist/windows ]; then
        rm -r dist/windows
        mkdir dist/windows
    else
        mkdir dist/windows
    fi

    # if [ -d dist/windows/cpp_headers ]; then
    #     rm -r dist/windows/cpp_headers
    #     mkdir dist/windows/cpp_headers
    # else
    #     mkdir dist/windows/cpp_headers
    # fi
    
    cp -r dist/distribute_assets dist/windows/assets
    cp -r imgui.ini dist/windows/imgui.ini
    cp -r shaders dist/windows/shaders
    cp -r build dist/windows/build 
    cp -r cs-assembly dist/windows/cs-assembly 
    cp -r windows/lib dist/windows/lib
    cp -r mono/lib dist/windows/lib/mono
    cp -r windows/bin dist/windows/bin
    cp -r bin/game.out dist/windows/bin/game.out
    cp -r LaunchGame.sh dist/windows/bin/LaunchGame.sh
    cp windows/Vault\ Engine.bat dist/windows/Vault\ Engine.bat
    cp -r cxx dist/windows/cxx
elif [ "$PLATFORM" == "linux_install" ]; then
     if [ -d /opt/vault-engine ]; then
        rm -r /opt/vault-engine
        mkdir /opt/vault-engine
    else
        mkdir /opt/vault-engine
    fi

    if [ -d /opt/vault-engine/bin ]; then
        rm -r /opt/vault-engine/bin
        mkdir /opt/vault-engine/bin
    else
        mkdir /opt/vault-engine/bin
    fi

    cp -r lib /opt/vault-engine/lib
    cp -r bin/build.out /opt/vault-engine/bin/build.out
    cp -r bin/game.out /opt/vault-engine/bin/game.out
    cp -r bin/build_proj.out /opt/vault-engine/bin/build_proj.out
    cp -r windows/lib /opt/vault-engine/bin/dlls
    cp -r windows/bin/game.exe /opt/vault-engine/bin/game.exe
    cp -r windows/LaunchGame.bat /opt/vault-engine/bin/LaunchGame.bat
    cp -r LaunchGame.sh /opt/vault-engine/bin/LaunchGame.sh
    cp -r Vault\ Engine.sh /opt/vault-engine/vault.sh
    cp -r vault.desktop /usr/share/applications/vault.desktop

    cp -r dist/distribute_assets /opt/vault-engine/assets
    cp -r imgui.ini /opt/vault-engine/imgui.ini
    cp -r shaders /opt/vault-engine/shaders
    cp -r build /opt/vault-engine/build
    cp -r mono /opt/vault-engine/mono
    cp -r cs-assembly /opt/vault-engine/cs-assembly
    cp -r cxx /opt/vault-engine/cxx
else 
    if [ -d dist/linux ]; then
        rm -r dist/linux
        mkdir dist/linux
    else
        mkdir dist/linux
    fi

    if [ -d dist/linux/bin ]; then
        rm -r dist/linux/bin
        mkdir dist/linux/bin
    else
        mkdir dist/linux/bin
    fi

    cp -r lib dist/linux/lib
    cp -r bin/build.out dist/linux/bin/build.out
    cp -r bin/game.out dist/linux/bin/game.out
    cp -r bin/build_proj.out dist/linux/bin/build_proj.out
    cp -r windows/lib dist/linux/bin/dlls
    cp -r windows/bin/game.exe dist/linux/bin/game.exe
    cp -r windows/LaunchGame.bat dist/linux/bin/LaunchGame.bat
    cp -r LaunchGame.sh dist/linux/bin/LaunchGame.sh
    cp -r Vault\ Engine.sh dist/linux/Vault\ Engine.sh

    cp -r dist/distribute_assets dist/linux/assets
    cp -r imgui.ini dist/linux/imgui.ini
    cp -r shaders dist/linux/shaders
    cp -r build dist/linux/build
    cp -r mono dist/linux/mono
    cp -r cs-assembly dist/linux/cs-assembly
    cp -r cxx dist/linux/cxx
fi