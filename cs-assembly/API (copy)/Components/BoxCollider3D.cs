using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class BoxCollider3D : Component
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_GetVec3(string id, out string result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_SetVec3(string id, float x, float y, float z);


        public Vector3 size
        {
            get
            {
                cpp_GetVec3(Entity.ID, out string result);
                string[] strs = result.Split(" ");
                return new Vector3(float.Parse(strs[0]), float.Parse(strs[1]), float.Parse(strs[2]));
            }
            set
            {
                cpp_SetVec3(Entity.ID, value.x, value.y, value.z);
            }
        }
    }
}