using System;
using Vault;

namespace MiroGayBoy
{
    public class GayBoy : Entity
    {
        void OnStart()
        {
            SetObjectID();
        }

        void OnUpdate()
        {
            Debug.Log("Miro is very gay boy!");
            Vector3 vec3 = new Vector3(1, 1, 1);
            Vector3 vec32 = new Vector3(1, 1, 1);
        }
    }
}