using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class Material : Component
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void GetColor(string ID, string Component, out string result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void SetColor(string ID, string Component, float r, float g, float b, float a);

        public Vector4 color
        {
            get
            {
                GetColor(Entity.ID, "MeshRenderer", out string result);
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
                SetColor(Entity.ID, "MeshRenderer", value.x, value.y, value.z, value.w);
            }
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void GetTexture(string ID, string Component, string type, out string texture);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void SetTexture(string ID, string Component, string type, string texture);

        public string diffuse
        {
            get
            {
                GetTexture(Entity.ID, "MeshRenderer", "diffuse", out string texture);
                return texture;
            }
            set
            {
                SetTexture(Entity.ID, "MeshRenderer", "diffuse", value);
            }
        }
        public string specular
        {
            get
            {
                GetTexture(Entity.ID, "MeshRenderer", "specular", out string texture);
                return texture;
            }
            set
            {
                SetTexture(Entity.ID, "MeshRenderer", "specular", value);
            }
        }
        public string normal
        {
            get
            {
                GetTexture(Entity.ID, "MeshRenderer", "normal", out string texture);
                return texture;
            }
            set
            {
                SetTexture(Entity.ID, "MeshRenderer", "normal", value);
            }
        }
        public string emission
        {
            get
            {
                GetTexture(Entity.ID, "MeshRenderer", "emission", out string texture);
                return texture;
            }
            set
            {
                SetTexture(Entity.ID, "MeshRenderer", "emission", value);
            }
        }

        // Texture Scale
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void GetTextureScale(string ID, out string result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void SetTextureScale(string ID, float x, float y);

        // Metallic
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void GetMetallic(string ID, out string result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void SetMetallic(string ID, float value);

        // Roughness
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void GetRoughness(string ID, out string result);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void SetRoughness(string ID, float value);

        public Vector2 texScale
        {
            get
            {
                GetTextureScale(Entity.ID, out string result);
                string[] splitted = result.Split(" ");
                return new Vector2(float.Parse(splitted[0]), float.Parse(splitted[0]));
            }
            set
            {
                SetTextureScale(Entity.ID, value.x, value.y);
            }
        }

        public float metallic
        {
            get
            {
                GetMetallic(Entity.ID, out string result);
                return float.Parse(result);
            }
            set
            {
                SetMetallic(Entity.ID, value);
            }
        }
        public float roughness
        {
            get
            {
                GetRoughness(Entity.ID, out string result);
                return float.Parse(result);
            }
            set
            {
                SetRoughness(Entity.ID, value);
            }
        }

        public CustomShader customShader;
    }
}