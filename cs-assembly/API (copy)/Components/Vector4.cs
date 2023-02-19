using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class Vector4
    {
        public float x, y, z, w;

        public Vector4(float X, float Y, float Z, float W)
        {
            x = X;
            y = Y;
            z = Z;
            w = W;
        }

        public void set(Vector4 val)
        {
            x = val.x;
            y = val.y;
            z = val.z;
            w = val.w;
        }
    }
}