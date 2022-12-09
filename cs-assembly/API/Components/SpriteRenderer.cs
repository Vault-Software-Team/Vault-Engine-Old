using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class SpriteRenderer : Component
    {
        public Vector4 color;

        public string texture;

        public CustomShader customShader;
    }
}