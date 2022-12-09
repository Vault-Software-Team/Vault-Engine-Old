using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class Vector3
    {
        public float x, y, z;

        public Vector3(float X, float Y, float Z)
        {
            x = X;
            y = Y;
            z = Z;
        }

        public void set(Vector3 val)
        {
            x = val.x;
            y = val.y;
            z = val.z;
        }
    }
}