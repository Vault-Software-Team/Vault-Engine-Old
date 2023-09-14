using System.Runtime.Intrinsics.X86;
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

    public class Discord
    {
        public Discord() { }
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Init(string client_id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void SetPresence(string details, string state, string largeImageKey, string largeImageText, string smallImageKey, string smallImageText);
    }

    public class Audio
    {
        public Audio() { }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [Obsolete("Audio.Play is obselete (it is 2D only), use Audio3D Component instead to play audio")]
        extern public static void Play(string file, float volume, bool loop, int channel = -1);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [Obsolete("Audio.Music is obselete (it is 2D only), use Audio3D Component instead to play audio")]
        extern public static void Music(string file, float volume, bool loop);
    }

    public class Main
    {
        public static float PI = 3.14159265358979f;

        public Main()
        {
            Vector3 vector = new Vector3(1, 2, 3);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float cpp_DeltaTime();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float ExitProgram();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float cpp_Ambient(bool toGet = true, float value = 0);

        public static float ToRad(float degrees)
        {
            return degrees * (PI / 180);
        }

        public static float deltaTime
        {
            get { return cpp_DeltaTime(); }
            set
            { }
        }

        public static float ambient
        {
            get { return cpp_Ambient(); }
            set { cpp_Ambient(false, value); }
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
        public static int KEY_ESCAPE = 256;
        public static int KEY_ENTER = 257;
        public static int KEY_TAB = 258;
        public static int KEY_BACKSPACE = 259;
        public static int KEY_INSERT = 260;
        public static int KEY_DELETE = 261;
        public static int KEY_RIGHT = 262;
        public static int KEY_LEFT = 263;
        public static int KEY_DOWN = 264;
        public static int KEY_UP = 265;
        public static int KEY_PAGE_UP = 266;
        public static int KEY_PAGE_DOWN = 267;
        public static int KEY_HOME = 268;
        public static int KEY_END = 269;
        public static int KEY_CAPS_LOCK = 280;
        public static int KEY_SCROLL_LOCK = 281;
        public static int KEY_NUM_LOCK = 282;
        public static int KEY_PRINT_SCREEN = 283;
        public static int KEY_PAUSE = 284;
        public static int KEY_F1 = 290;
        public static int KEY_F2 = 291;
        public static int KEY_F3 = 292;
        public static int KEY_F4 = 293;
        public static int KEY_F5 = 294;
        public static int KEY_F6 = 295;
        public static int KEY_F7 = 296;
        public static int KEY_F8 = 297;
        public static int KEY_F9 = 298;
        public static int KEY_F10 = 299;
        public static int KEY_F11 = 300;
        public static int KEY_F12 = 301;
        public static int KEY_F13 = 302;
        public static int KEY_F14 = 303;
        public static int KEY_F15 = 304;
        public static int KEY_F16 = 305;
        public static int KEY_F17 = 306;
        public static int KEY_F18 = 307;
        public static int KEY_F19 = 308;
        public static int KEY_F20 = 309;
        public static int KEY_F21 = 310;
        public static int KEY_F22 = 311;
        public static int KEY_F23 = 312;
        public static int KEY_F24 = 313;
        public static int KEY_F25 = 314;
        public static int KEY_KP_0 = 320;
        public static int KEY_KP_1 = 321;
        public static int KEY_KP_2 = 322;
        public static int KEY_KP_3 = 323;
        public static int KEY_KP_4 = 324;
        public static int KEY_KP_5 = 325;
        public static int KEY_KP_6 = 326;
        public static int KEY_KP_7 = 327;
        public static int KEY_KP_8 = 328;
        public static int KEY_KP_9 = 329;
        public static int KEY_KP_DECIMAL = 330;
        public static int KEY_KP_DIVIDE = 331;
        public static int KEY_KP_MULTIPLY = 332;
        public static int KEY_KP_SUBTRACT = 333;
        public static int KEY_KP_ADD = 334;
        public static int KEY_KP_ENTER = 335;
        public static int KEY_KP_EQUAL = 336;
        public static int KEY_LEFT_SHIFT = 340;
        public static int KEY_LEFT_CONTROL = 341;
        public static int KEY_LEFT_ALT = 342;
        public static int KEY_LEFT_SUPER = 343;
        public static int KEY_RIGHT_SHIFT = 344;
        public static int KEY_RIGHT_CONTROL = 345;
        public static int KEY_RIGHT_ALT = 346;
        public static int KEY_RIGHT_SUPER = 347;
        public static int KEY_MENU = 348;

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
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_GetMouseWorldPosition(out string result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static float atan2(float x, float y);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_normalize(float x, float y, float z, out string result);

        public static Vector3 normalize(Vector3 vec)
        {
            cpp_normalize(vec.x, vec.y, vec.z, out string result);
            string[] results = result.Split(" ");
            return new Vector3(float.Parse(results[0]), float.Parse(results[1]), float.Parse(results[2]));
        }

        public static Vector3 GetMouseWorldPosition()
        {
            cpp_GetMouseWorldPosition(out string result);
            string[] results = result.Split(" ");
            return new Vector3(float.Parse(results[0]), float.Parse(results[1]), float.Parse(results[2]));
        }

    }

    public class Lerp
    {
        public static float Float(float a, float b, float t)
        {
            return a + t * (b - a);
        }
    }

    public class Entity
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void GetID(out string result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_GetDataFromID(string id, out string result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static bool cpp_GetEnabled(string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_SetEnabled(string id, bool value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_AddComponent(string id, string type);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_RemoveGO(string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static object cpp_GetClassInstance(string id, string type);

        public string parentID = "NO_PARENT";
        public string ID = "";
        public string name = "";
        public string tag = "";

        public bool enabled
        {
            get
            {
                return cpp_GetEnabled(ID);
            }
            set
            {
                cpp_SetEnabled(ID, value);
            }
        }

        public Transform transform;

        protected virtual void OnStart() { }
        protected virtual void OnUpdate(float ts) { }
        protected virtual void OnGUI() { }
        protected virtual void OnCollisionEnter2D(string ID) { }
        protected virtual void OnCollisionExit2D(string ID) { }
        protected virtual void OnCollisionEnter3D(string ID) { }
        protected virtual void OnCollisionExit3D(string ID) { }
        protected virtual void OnMouseEnter() { }
        protected virtual void OnMouseExit() { }

        protected void SetObjectID()
        {
            transform = GetComponent<Transform>();
            GetID(out string result);
            string[] data = result.Split(",");
            ID = data[0];
            name = data[1];
            tag = data[2];
        }

        public Entity GetEntity(string m_id)
        {
            cpp_GetDataFromID(m_id, out string result);
            string[] data = result.Split(",");
            return new Entity()
            {
                ID = data[0],
                name = data[1],
                tag = data[2],
            };
        }

        public void RemoveEntity(string m_id)
        {
            cpp_RemoveGO(m_id);
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

        public T? As<T>() where T : Entity, new()
        {
            object instance = cpp_GetClassInstance(ID, typeof(T).Name);
            if (instance == null) return null;
            return instance as T;
        }
    }

    public class Scene
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static string LoadScene(string file_path);
    }

    public class GameObject
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static string GetIDByName(string name);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static string GetIDByTag(string tag);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static string cpp_AddGameObject(string name, string tag, out string m_id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static string cpp_RemoveGameObject(string m_id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static string cpp_InstantiatePrefab(string prefab_path);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static string cpp_InstantiatePrefabWithProperties(string prefab_path, float px, float py, float pz, float rx, float ry, float rz, string parent_id = "NO_PARENT", string object_name = "Prefab", string object_tag = "Untagged", string custom_uuid = "GENERATE_RANDOM");

        public static void RemoveGameObject(GameObject game_object)
        {
            cpp_RemoveGameObject(game_object.id);
        }

        public static void RemoveGameObject(string id)
        {
            cpp_RemoveGameObject(id);
        }

        public static void InstantiatePrefab(string prefab_path)
        {
            cpp_InstantiatePrefab(prefab_path);
        }

        public static void InstantiatePrefabWithProperties(string prefab_path, Vector3 position, Vector3 rotation, string parent_id = "NO_PARENT", string object_name = "Prefab", string object_tag = "Untagged", string custom_uuid = "GENERATE_RANDOM")
        {
            cpp_InstantiatePrefabWithProperties(prefab_path, position.x, position.y, position.z, rotation.x, rotation.y, rotation.z, parent_id, object_name, object_tag, custom_uuid);
        }

        public string name;
        public string tag;
        public string id;
        public Entity entity;

        public GameObject(string m_name = "GameObject (C#)", string m_tag = "Default")
        {
            cpp_AddGameObject(m_name, m_tag, out string m_id);
            name = m_name;
            tag = m_tag;
            id = m_id;
            entity = new Entity() { ID = id };
        }
    }
}