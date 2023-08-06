using System;
using Vault;

namespace Classes
{
    class Spy : Entity
    {
        Transform transform;
        public float speed = 2;

        void OnStart()
        {
            SetObjectID();

            transform = GetComponent<Transform>();
        }

        void OnUpdate()
        {
            Vector3 rot = transform.rotation;
            rot.y += speed * Main.deltaTime;
            transform.rotation = rot;
        }
    }
}