using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class SpritesheetAnimation : Component
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static void GetCurrentAnimation(string ID, out string result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static void SetCurrentAnimation(string ID, string value);

        public Vector4 color
        {
            get
            {
                Material.GetColor(Entity.ID, "SpritesheetAnimation", out string result);
                string[] values = result.Split(" ");
                return new Vector4(
                    float.Parse(values[0]),
                    float.Parse(values[1]),
                    float.Parse(values[2]),
                    float.Parse(values[3])
                );
            }
            set
            {
                Material.SetColor(Entity.ID, "SpritesheetAnimation", value.x, value.y, value.z, value.w);
            }
        }

        public string currentAnimation
        {
            get
            {
                GetCurrentAnimation(Entity.ID, out string result);
                return result;
            }
            set
            {
                SetCurrentAnimation(Entity.ID, value);
            }
        }

        public string texture
        {
            get
            {
                Material.GetTexture(Entity.ID, "SpritesheetAnimation", "diffuse", out string texture);
                return texture;
            }
            set
            {
                Material.SetTexture(Entity.ID, "SpritesheetAnimation", "diffuse", value);
            }
        }

        public CustomShader customShader;
    }
}