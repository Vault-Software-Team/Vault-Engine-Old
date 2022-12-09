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
        public Vector3 vec3;
        void OnStart()
        {
            GetID(out string result);
            ID = result;
        }

        void OnUpdate(float ts)
        {
            Transform comp = GetComponent<Transform>();
            Vector3 vec3 = comp.position;
            comp.position = new Vector3(1, 1, 1);
        }
    }
}
