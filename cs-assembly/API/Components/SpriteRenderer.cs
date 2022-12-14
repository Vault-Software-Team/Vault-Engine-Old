using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class SpriteRenderer : Component
    {
        public Vector4 color
        {
            get
            {
                Material.GetColor(Entity.ID, "SpriteRenderer", out string result);
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
                Material.SetColor(Entity.ID, "SpriteRenderer", value.x, value.y, value.z, value.w);
            }
        }

        public string texture
        {
            get
            {
                Material.GetTexture(Entity.ID, "SpriteRenderer", "diffuse", out string texture);
                return texture;
            }
            set
            {
                Material.SetTexture(Entity.ID, "SpriteRenderer", "diffuse", value);
            }
        }

        public CustomShader customShader;
    }
}