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
                Scene.LoadScene("assets/scenes/silver.vault");
                clicked = true;
            }

            if (Input.IsKeyReleased(Input.KEY_Q) && clicked) clicked = false;
        }
    }
}
