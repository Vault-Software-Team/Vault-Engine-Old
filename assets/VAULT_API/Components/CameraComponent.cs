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
    }
}