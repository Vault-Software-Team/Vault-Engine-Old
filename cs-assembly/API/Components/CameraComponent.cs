using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class CameraComponent : Component
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_GetKey(string key, string id, out string result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_SetKey(string key, float value, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_Set2D(bool value, string id);

        public float fov
        {
            get
            {
                cpp_GetKey("fov", Entity.ID, out string result);
                return float.Parse(result);
            }
            set
            {
                cpp_SetKey("fov", value, Entity.ID);
            }
        }

        public float near
        {
            get
            {
                cpp_GetKey("near", Entity.ID, out string result);
                return float.Parse(result);
            }
            set
            {
                cpp_SetKey("near", value, Entity.ID);
            }
        }

        public float far
        {
            get
            {
                cpp_GetKey("far", Entity.ID, out string result);
                return float.Parse(result);
            }
            set
            {
                cpp_SetKey("far", value, Entity.ID);
            }
        }

        public bool mode2D
        {
            get
            {
                cpp_GetKey("far", Entity.ID, out string result);
                return result == "true" ? true : false;
            }
            set
            {
                cpp_Set2D(value, Entity.ID);
            }
        }

        public bool mouse_movement
        {
            get
            {
                cpp_GetKey("mouse_movement", Entity.ID, out string result);
                return result == "true" ? true : false;
            }
            set
            {
                cpp_SetKey("mouse_movement", value == true ? 1.0f : 0.0f, Entity.ID);
            }
        }

        public Vector3 up
        {
            get
            {
                cpp_GetKey("up", Entity.ID, out string result);
                string[] strValues = result.Split(" ");
                return new Vector3(float.Parse(strValues[0]), float.Parse(strValues[1]), float.Parse(strValues[2]));
            }
            set { }
        }

        public Vector3 right
        {
            get
            {
                cpp_GetKey("right", Entity.ID, out string result);
                string[] strValues = result.Split(" ");
                return new Vector3(float.Parse(strValues[0]), float.Parse(strValues[1]), float.Parse(strValues[2]));
            }
            set { }
        }

        public Vector3 forward
        {
            get
            {
                cpp_GetKey("forward", Entity.ID, out string result);
                string[] strValues = result.Split(" ");
                return new Vector3(float.Parse(strValues[0]), float.Parse(strValues[1]), float.Parse(strValues[2]));
            }
            set { }
        }
    }
}