using System;
using Vault;

namespace Sandbox
{

    public class TestVec3
    {
        public float x;
        public float y;
        public float z;

        public TestVec3(float _x, float _y, float _z)
        {
            x = _x;
            y = _y;
            z = _z;
        }
    }

    public class Player : Entity
    {
        private float speed = 5;

        void OnStart()
        {
            GetID(out string result);
            ID = result;
        }

        void OnUpdate(float ts)
        {
            MeshRenderer renderer = GetComponent<MeshRenderer>();
            Material.GetMetallic(ID, out string result);
            Console.WriteLine(result);
        }
    }
}
