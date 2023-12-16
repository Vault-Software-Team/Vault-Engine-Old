using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class Text3D : Component
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void GetKey(string key, string id, out string result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void SetText(string text, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void SetOthers(float value, string key, string id);

        public string text 
        {
            get 
            {
                GetKey("text", Entity.ID, out string result);
                return result;
            } 
            set 
            {
                SetText(value, Entity.ID);
            }
        }

        public float scale 
        {
            get 
            {
                GetKey("scale", Entity.ID, out string result);
                return float.Parse(result);
            }
            set 
            {
                SetOthers(value, "scale", Entity.ID);
            }
        }

        public float y_offset 
        {
            get 
            {
                GetKey("y_offset", Entity.ID, out string result);
                return float.Parse(result);
            }
            set 
            {
                SetOthers(value, "y_offset", Entity.ID);
            }
        }
    }
}