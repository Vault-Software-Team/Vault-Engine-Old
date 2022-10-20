<div align="center">
<img src="build/logo_github.png" width="500px">

---

<h3>An Open Source 2D and 3D Game Engine</h3>
<p align="center">Built with C++</p>
</div>

<br>
<br>

## Supported Platforms:

-   **Linux**
-   **Windows**

## Features

-   **2D and 3D Rendering**
-   **Script Engine: Lua**
-   **Physics Engine: Box2D, Bullet Physics (3D)**
-   **Audio: SDL_mixer**
-   **Level Editor**
-   **Native Scripting**
-   **2D Lighting**
-   **3D Lighting**
-   **More coming soon**

## Used Dependencies

-   **Assimp**
-   **Box2D**
-   **Discord RPC**
-   **Freetype**
-   **ImGuizmo**
-   **EnTT**
-   **GLAD**
-   **GLFW**
-   **glm**
-   **IconFontCppHeaders**
-   **ImGui**
-   **ImGuiFileDialog**
-   **nlohmann json**
-   **SDL/SDL_mixer**
-   **stb_image**
-   **tinyxml**
-   **Bullet Physics**
-   **OpenAL**
-   **luajit**

# Installation

### **Linux**

```bash
git clone https://github.com/koki10190/Vault-Engine.git
cd Vault-Engine
make linux
./Vault\ Engine.sh
```

### **Windows**

First! Go to Makefile and change x86_64-w64-mingw32-g++ to the g++ version you have installed (MUST HAVE MINGW INSTALLED FOR THIS AND MINGW MAKE, you do not have to change this if you are on linux and want to compile it for windows, you just need to install x86_64-w64-mingw32)

```bash
git clone https://github.com/koki10190/Vault-Engine.git
cd Vault-Engine
make win
./win_build.exe
```

### **Bundling**

Bundling the application is very easy, this only works on linux since i do not have .bat file for this created

```bash
./bundle.sh # this will bundle for linux, in dist/linux
./bundle.sh windows # this will bundle for windows, in dist/windows
```
NOTE: When you bundle the application, building will not work!
This will be changed in the future when I'll add a project system to the engine

# Upcoming Features

-  **Vulkan Rendering** 
-  **Project System**
-  **JavaScript Scripting**
-  **Python Scripting**
-  **Full C++ Scripting support**
-  **Asset Packager**
-  **Android Support**
-  **Flowchart Editor (like in Unreal Engine)**
-  **Video Player**