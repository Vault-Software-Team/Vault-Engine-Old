using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class Material : Component
    {
        public Vector4 color;

        public string diffuse;
        public string specular;
        public string normal;
        public string emission;

        public CustomShader customShader;
    }
}