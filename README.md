<div style="
display: flex;
justify-content: center;
align-items: center;
flex-direction: column;
">
<img style="" src="./build/logo2.png">

<p style="width: 100%; text-align: center; margin-top: 40px; font-size: 50px; color: #ff3b3b; margin-bottom: -10px">Static Engine</p>

## An Open Source 2D and 3D Game Engine

### Built with C++

</div>

---

## Supported Platforms:

-   **Linux**

## Features

-   **2D and 3D Rendering**
-   **Script Engine: Lua**
-   **Physics Engine: Box2D, Bullet Physics (3D)**
-   **Audio (3D): OpenAL, (2D) SDL_mixer**
-   **Level Editor**
-   **Native Scripting**
-   **2D Lighting**
-   **3D Lighting**
-   **More coming soon**

## Used Dependencies

-   **Assimp**
-   **Box2D**
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

**Required Dependencies**

-   GLFW
-   SDL2
-   SDL_mixer
-   Box2D
-   Bullet Physics
-   Assimp
-   GLEW \
    other dependencies are in vendor folders

```bash
git clone https://github.com/koki10190/Static-Engine.git
cd Static-Engine
cd api
./make.sh
cd ../
make
```

Another way is:

```bash
git clone https://github.com/koki10190/Static-Engine.git
cd Static-Engine
./build.sh
```

# Upcoming Features

-   **Vulkan Rendering**
-   **Android Support (not for making games)**
-   **Flowchart Editor (like in Unreal Engine)**
-   **Video Player**

# How to run on Windows

1. Install <a href="https://docs.microsoft.com/en-us/windows/wsl/install-win10">Windows Subsystem for Linux</a>
2. Install <a href="https://www.microsoft.com/en-us/p/ubuntu/9nblggh4msv6?activetab=pivot:overviewtab">Ubuntu</a> from Microsoft Store
3. Install C++ compiler
4. Compile the Static Engine using the Ubuntu bash
5. Run the main.sh file using the Ubuntu bash (DO NOT RUN THE EXECUTABLE DIRECTLY)
