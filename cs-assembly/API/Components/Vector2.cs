using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class Vector2
    {
        public float x, y;

        public Vector2(float X, float Y)
        {
            x = X;
            y = Y;
        }

        public void set(Vector2 val)
        {
            x = val.x;
            y = val.y;
        }
    }
}