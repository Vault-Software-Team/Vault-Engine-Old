using System;
using Vault;

namespace Sandbox
{
    public class Player : Entity
    {
        private float jump_force = 50.0f;
        private float speed = 1.5f;
        private Vector3 vec = null;

        Rigidbody2D rigidbody;

        void OnStart()
        {
            SetObjectID();

            // rigidbody = GetComponent<Rigidbody2D>();
        }

        private bool jumped = false;
        void OnUpdate(float ts)
        {
            Console.WriteLine(vec.x);
            // Console.WriteLine("haiii! ^_^");
        }
    }
}
