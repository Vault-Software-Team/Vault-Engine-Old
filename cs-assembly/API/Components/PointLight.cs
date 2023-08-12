using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class PointLight : Component
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void GetKey(string key, string id, out string result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void SetKey(string key, string id, float x, float y, float z);

        public Vector3 color
        {
            get
            {
                GetKey("color", Entity.ID, out string result);
                string[] strValues = result.Split(" ");

                return new Vector3(float.Parse(strValues[0]), float.Parse(strValues[1]), float.Parse(strValues[2]));
            }
            set
            {
                SetKey("color", Entity.ID, value.x, value.y, value.z);
            }
        }

        public float intensity
        {
            get
            {
                GetKey("intensity", Entity.ID, out string result);
                return float.Parse(result);
            }
            set
            {
                SetKey("intensity", Entity.ID, value, 0, 0);
            }
        }
    }

}