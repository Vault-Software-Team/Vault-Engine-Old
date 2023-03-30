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

        void OnUpdate(float ts)
        {

        }

        bool entered;
        void OnMouseEnter()
        {
            if (entered) return;
            Debug.Log("Mouse Entered!");
            entered = true;
        }

        void OnMouseExit()
        {
            entered = false;
            Debug.Log("Mouse Exited!");
        }
    }
}
