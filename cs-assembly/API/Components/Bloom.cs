using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class Bloom : Component
    {
        public Vector3 bloomColor;

        public Bloom(Vector3 c)
        {
            bloomColor = c;
        }
    }
}