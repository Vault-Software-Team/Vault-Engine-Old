using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class Audio3D : Component
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_GetKey(string key, string id, out string result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_SetFloatKey(string key, string id, float val);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_SetVelocity(string id, float x, float y, float z);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static bool cpp_GetLoop(string id);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_SetLoop(string id, bool val);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_Play(string id);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_Stop(string id);


        public float volume
        {
            get
            {
                cpp_GetKey("volume", Entity.ID, out string result);
                return float.Parse(result);
            }
            set
            {
                cpp_SetFloatKey("volume", Entity.ID, value);
            }
        }

        public float pitch
        {
            get
            {
                cpp_GetKey("pitch", Entity.ID, out string result);
                return float.Parse(result);
            }
            set
            {
                cpp_SetFloatKey("pitch", Entity.ID, value);
            }
        }

        public float max_distance
        {
            get
            {
                cpp_GetKey("max_distance", Entity.ID, out string result);
                return float.Parse(result);
            }
            set
            {
                cpp_SetFloatKey("max_distance", Entity.ID, value);
            }
        }

        public bool loop
        {
            get
            {
                return cpp_GetLoop(Entity.ID);
            }
            set
            {
                cpp_SetLoop(Entity.ID, value);
            }
        }

        public Vector3 velocity
        {
            get
            {
                cpp_GetKey("velocity", Entity.ID, out string result);
                string[] vals = result.Split(" ");
                return new Vector3(float.Parse(vals[0]), float.Parse(vals[1]), float.Parse(vals[2]));
            }
            set
            {
                cpp_SetVelocity(Entity.ID, value.x, value.y, value.z);
            }
        }
    }
}