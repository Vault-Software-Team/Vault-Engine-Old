using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class SpriteAnimation : Component
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void GetKey(string key, string id, out string result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void SetCurrAnimation(string anim, string id);

        public string currentAnimation 
        {
            get
            {
                GetKey("currentAnimation", Entity.ID, out string result);
                return result;
            }
            set 
            {
                SetCurrAnimation(value, Entity.ID);
            }
        }
    }
}