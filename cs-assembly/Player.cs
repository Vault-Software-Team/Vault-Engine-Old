using System;
using Vault;

namespace Sandbox
{
    public class Player : Entity
    {
        Transform transform;

        void OnStart()
        {
            SetObjectID();
            transform = GetComponent<Transform>();
        }

        void OnUpdate(float ts)
        {
            Vector3 ray = Input.GetMouseWorldPosition();
            Vector3 difference = new Vector3(ray.x - transform.position.x, ray.y - transform.position.y, ray.z - transform.position.z);
            float rotZ = Input.atan2(difference.x, difference.y);
            transform.rotation = new Vector3(0, 0, -rotZ);
        }
    }
}
