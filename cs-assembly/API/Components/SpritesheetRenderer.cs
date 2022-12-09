using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class SpritesheetRenderer : Component
    {
        public Vector4 color;

        public string currentAnimation;
        public string texture;

        public CustomShader customShader;
    }
}