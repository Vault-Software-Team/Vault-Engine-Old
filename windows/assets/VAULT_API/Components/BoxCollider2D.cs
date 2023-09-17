using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class BoxCollider2D : Component
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_GetKey(string key, string id, out string result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_SetKey(string key, float value, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_SetOffset(float x, float y, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_SetSize(float x, float y, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static bool cpp_GetTrigger(string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_SetTrigger(string id, bool val);

        public float density 
        {
            get
            {
                cpp_GetKey("density", Entity.ID, out string result);
                return float.Parse(result);
            }
            set
            {
                cpp_SetKey("density", value, Entity.ID);
            }
        }

        public float friction
        {
            get
            {
                cpp_GetKey("friction", Entity.ID, out string result);
                return float.Parse(result);
            }
            set
            {
                cpp_SetKey("friction", value, Entity.ID);
            }
        }

        public float restitution
        {
            get
            {
                cpp_GetKey("restitution", Entity.ID, out string result);
                return float.Parse(result);
            }
            set
            {
                cpp_SetKey("restitution", value, Entity.ID);
            }
        }
        
        public float restitutionThreshold
        {
            get
            {
                cpp_GetKey("restitutionThreshold", Entity.ID, out string result);
                return float.Parse(result);
            }
            set
            {
                cpp_SetKey("restitutionThreshold", value, Entity.ID);
            }
        }

        public bool trigger
        {
            get
            {
                return cpp_GetTrigger(Entity.ID);
            }
            set
            {
                cpp_SetTrigger(Entity.ID, value);
            }
        }
    }
}