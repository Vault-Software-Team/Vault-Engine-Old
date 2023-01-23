using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class Bloom : Component
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void GetColor(string id, out string result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void SetColor(float x, float y, float z, string id);

        public Vector3 color
        {
            get 
            {
                GetColor(Entity.ID, out string result);

                string[] strValues = result.Split(" ");
                return new Vector3(float.Parse(strValues[0]), float.Parse(strValues[1]), float.Parse(strValues[2]));
            }
            set 
            {
                SetColor(value.x, value.y, value.z, Entity.ID);
            }
        }
    }
}