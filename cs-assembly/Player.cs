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
        }
    }
}