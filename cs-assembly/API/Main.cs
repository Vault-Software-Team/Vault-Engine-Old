using System.Runtime.CompilerServices;
using System;

namespace Vault
{
    public class Terminal
    {
        public Terminal() { }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Log(string text);
    }

    public class Debug
    {
        public Debug() { }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Log(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Error(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Warning(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void TestVector(ref Vector3 parameter);
    }

    public class Audio
    {
        public Audio() { }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Play(string file, float volume, bool loop, int channel = -1);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Music(string file, float volume, bool loop);
    }

    public class Main
    {
        public Main()
        {
            Vector3 vector = new Vector3(1, 2, 3);
        }
    }

    public class Input
    {
        public static int KEY_SPACE = 32;
        public static int KEY_APOSTROPHE = 39;  /* ' */
        public static int KEY_COMMA = 44;  /* , */
        public static int KEY_MINUS = 45;  /* - */
        public static int KEY_PERIOD = 46;  /* . */
        public static int KEY_SLASH = 47;  /* / */
        public static int KEY_0 = 48;
        public static int KEY_1 = 49;
        public static int KEY_2 = 50;
        public static int KEY_3 = 51;
        public static int KEY_4 = 52;
        public static int KEY_5 = 53;
        public static int KEY_6 = 54;
        public static int KEY_7 = 55;
        public static int KEY_8 = 56;
        public static int KEY_9 = 57;
        public static int KEY_SEMICOLON = 59;  /* ; */
        public static int KEY_EQUAL = 61;  /* = */
        public static int KEY_A = 65;
        public static int KEY_B = 66;
        public static int KEY_C = 67;
        public static int KEY_D = 68;
        public static int KEY_E = 69;
        public static int KEY_F = 70;
        public static int KEY_G = 71;
        public static int KEY_H = 72;
        public static int KEY_I = 73;
        public static int KEY_J = 74;
        public static int KEY_K = 75;
        public static int KEY_L = 76;
        public static int KEY_M = 77;
        public static int KEY_N = 78;
        public static int KEY_O = 79;
        public static int KEY_P = 80;
        public static int KEY_Q = 81;
        public static int KEY_R = 82;
        public static int KEY_S = 83;
        public static int KEY_T = 84;
        public static int KEY_U = 85;
        public static int KEY_V = 86;
        public static int KEY_W = 87;
        public static int KEY_X = 88;
        public static int KEY_Y = 89;
        public static int KEY_Z = 90;
        public static int KEY_LEFT_BRACKET = 91;  /* [ */
        public static int KEY_BACKSLASH = 92;  /* \ */
        public static int KEY_RIGHT_BRACKET = 93;  /* ] */
        public static int KEY_GRAVE_ACCENT = 96;  /* ` */
        public static int KEY_WORLD_1 = 61; /* non-US #1 */
        public static int KEY_WORLD_2 = 62; /* non-US #2 */

        /* Function keys */
        public static int KEY_ESCAPE = 56;
        public static int KEY_ENTER = 57;
        public static int KEY_TAB = 58;
        public static int KEY_BACKSPACE = 59;
        public static int KEY_INSERT = 60;
        public static int KEY_DELETE = 61;
        public static int KEY_RIGHT = 62;
        public static int KEY_LEFT = 63;
        public static int KEY_DOWN = 64;
        public static int KEY_UP = 65;
        public static int KEY_PAGE_UP = 66;
        public static int KEY_PAGE_DOWN = 67;
        public static int KEY_HOME = 68;
        public static int KEY_END = 69;
        public static int KEY_CAPS_LOCK = 80;
        public static int KEY_SCROLL_LOCK = 81;
        public static int KEY_NUM_LOCK = 82;
        public static int KEY_PRINT_SCREEN = 83;
        public static int KEY_PAUSE = 84;
        public static int KEY_F1 = 90;
        public static int KEY_F2 = 91;
        public static int KEY_F3 = 92;
        public static int KEY_F4 = 93;
        public static int KEY_F5 = 94;
        public static int KEY_F6 = 95;
        public static int KEY_F7 = 96;
        public static int KEY_F8 = 97;
        public static int KEY_F9 = 98;
        public static int KEY_F10 = 99;
        public static int KEY_F11 = 00;
        public static int KEY_F12 = 01;
        public static int KEY_F13 = 02;
        public static int KEY_F14 = 03;
        public static int KEY_F15 = 04;
        public static int KEY_F16 = 05;
        public static int KEY_F17 = 06;
        public static int KEY_F18 = 07;
        public static int KEY_F19 = 08;
        public static int KEY_F20 = 09;
        public static int KEY_F21 = 10;
        public static int KEY_F22 = 11;
        public static int KEY_F23 = 12;
        public static int KEY_F24 = 13;
        public static int KEY_F25 = 14;
        public static int KEY_KP_0 = 20;
        public static int KEY_KP_1 = 21;
        public static int KEY_KP_2 = 22;
        public static int KEY_KP_3 = 23;
        public static int KEY_KP_4 = 24;
        public static int KEY_KP_5 = 25;
        public static int KEY_KP_6 = 26;
        public static int KEY_KP_7 = 27;
        public static int KEY_KP_8 = 28;
        public static int KEY_KP_9 = 29;
        public static int KEY_KP_DECIMAL = 30;
        public static int KEY_KP_DIVIDE = 31;
        public static int KEY_KP_MULTIPLY = 32;
        public static int KEY_KP_SUBTRACT = 33;
        public static int KEY_KP_ADD = 34;
        public static int KEY_KP_ENTER = 35;
        public static int KEY_KP_EQUAL = 36;
        public static int KEY_LEFT_SHIFT = 40;
        public static int KEY_LEFT_CONTROL = 41;
        public static int KEY_LEFT_ALT = 42;
        public static int KEY_LEFT_SUPER = 43;
        public static int KEY_RIGHT_SHIFT = 44;
        public static int KEY_RIGHT_CONTROL = 45;
        public static int KEY_RIGHT_ALT = 46;
        public static int KEY_RIGHT_SUPER = 47;
        public static int KEY_MENU = 48;

        public static int KEY_LAST = 348;

        // controller keys
        public static int KEY_CONTROLLER_A = 0;
        public static int KEY_CONTROLLER_B = 1;
        public static int KEY_CONTROLLER_Y = 3;
        public static int KEY_CONTROLLER_X = 2;
        public static int KEY_CONTROLLER_L1 = 4;
        public static int KEY_CONTROLLER_R1 = 5;
        // start, select
        public static int KEY_CONTROLLER_START = 7;
        public static int KEY_CONTROLLER_SELECT = 6;
        // L3, R3
        public static int KEY_CONTROLLER_L3 = 9;
        public static int KEY_CONTROLLER_R3 = 10;
        // up, down, left, right
        public static int KEY_CONTROLLER_UP = 11;
        public static int KEY_CONTROLLER_DOWN = 13;
        public static int KEY_CONTROLLER_LEFT = 14;
        public static int KEY_CONTROLLER_RIGHT = 12;

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static bool IsKeyPressed(int key);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static bool IsKeyReleased(int key);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static bool IsMouseButtonPressed(int button);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static bool IsMouseButtonReleased(int button);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static int GetHorizontalAxis();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static int GetVerticalAxis();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static int GetMouseXAxis();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static int GetMouseYAxis();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void SetMouseHidden(bool hidden);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void SetMousePosition(float x, float y);
    }

    public class Lerp {
        public static float Float(float a, float b, float t) {
            return a + t * (b - a);
        }
    }

    public class Entity
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void GetID(out string result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_AddComponent(string id, string type);

        public string parentID = "NO_PARENT";
        public string ID;

        protected virtual void OnStart() { }
        protected virtual void OnUpdate(float ts) { }
        protected virtual void OnCollision2D(string ID) {}
        protected virtual void OnCollision3D(string ID) {}
        protected virtual void OnMouseEnter(string ID) {}
        protected virtual void OnMouseExit(string ID) {}

        protected void SetObjectID()
        {
            GetID(out string result);
            ID = result;
        }

        public Entity GetEntity(string m_id) {
            return new Entity() { ID = m_id };
        }

        public T GetComponent<T>() where T : Component, new()
        {
            T component = new T() { Entity = this };
            return (T)(object)component;

            return null;
        }

        public T AddComponent<T>() where T : Component, new()
        {
            cpp_AddComponent(ID, typeof(T).Name);
            return GetComponent<T>();
        }
    }

    public class GameObject {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static string GetIDByName(string name);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static string GetIDByTag(string tag);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static string cpp_AddGameObject(string name, string tag, out string m_id);
    
        public string name;
        public string tag;
        public string id;
        public Entity entity;

        public GameObject(string m_name = "GameObject (C#)", string m_tag = "Default") {
            cpp_AddGameObject(m_name, m_tag, out string m_id);
            name = m_name;
            tag = m_tag;
            id = m_id;
            entity = new Entity() { ID = id };
        } 
    }
}