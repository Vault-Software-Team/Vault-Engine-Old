using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class Transform : Component
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void GetKey(string key, string id, out string result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void SetKey(string key, string id, string value);

        public Vector3 position
        {
            get
            {
                GetKey("position", Entity.ID, out string result);
                string[] strValues = result.Split(" ");
                return new Vector3(float.Parse(strValues[0]), float.Parse(strValues[1]), float.Parse(strValues[2]));
            }
            set
            {
                string strValue = 5.ToString() + " " + value.y.ToString("R") + " " + value.z.ToString("R");
                System.Console.WriteLine(strValue);
                SetKey("position", Entity.ID, strValue);
            }
        }

        public Vector3 rotation = new Vector3(0, 0, 0);

        public Vector3 scale = new Vector3(0, 0, 0);
    }
}