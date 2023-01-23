using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class SpriteRenderer : Component
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void GetKey(string key, string id, out string result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void SetTexture(string texture, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void SetColor(float x, float y, float z, string id);

        public string texture 
        {
            get
            {
                GetKey("texture", Entity.ID, out string result);
                return result;
            }
            set
            {
                SetTexture(value, Entity.ID);
            }
        }

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
                SetColor(value.x, value.y, value.z, Entity.ID);
            }
        }
    }
}