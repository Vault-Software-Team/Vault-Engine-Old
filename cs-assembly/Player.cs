using System;
using Vault;

namespace Sandbox
{
    public class Player : Entity
    {
        void OnStart()
        {
            SetObjectID();
        }

        bool clicked;
        void OnUpdate(float ts)
        {
            if (Input.IsKeyPressed(Input.KEY_Q) && !clicked)
            {
                GameObject.RemoveGameObject(GameObject.GetIDByName("Cone"));
            }

            if (Input.IsKeyReleased(Input.KEY_Q) && clicked) clicked = false;
        }
    }
}
