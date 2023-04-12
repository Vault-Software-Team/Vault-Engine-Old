using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class Rigidbody2D : Component
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_GetKey(string key, string id, out string result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_SetType(int type, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_SetVelocity(float x, float y, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_SetAngularVelocity(float velocity, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_SetPosition(float x, float y, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_Force(float x, float y, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_Torque(float torque, string id);

        public void SetVelocity(float x, float y)
        {
            cpp_SetVelocity(x, y, Entity.ID);
        }

        public void SetAngularVelocity(float velocity)
        {
            cpp_SetAngularVelocity(velocity, Entity.ID);
        }

        public void SetPosition(float x, float y)
        {
            cpp_SetPosition(x, y, Entity.ID);
        }

        public void Force(float x, float y)
        {
            cpp_Force(x, y, Entity.ID);
        }

        public void Torque(float torque)
        {
            cpp_Torque(torque, Entity.ID);
        }

        public Vector2 velocity
        {
            get
            {
                cpp_GetKey("velocity", Entity.ID, out string result);
                string[] m_str = result.Split(" ");
                return new Vector2(float.Parse(m_str[0]), float.Parse(m_str[1]));
            }
            set
            {
                cpp_SetVelocity(value.x, value.y, Entity.ID);
            }
        }

        public int type
        {
            get
            {
                cpp_GetKey("type", Entity.ID, out string result);
                return int.Parse(result);
            }
            set
            {
                cpp_SetType(value, Entity.ID);
            }
        }
    }
}