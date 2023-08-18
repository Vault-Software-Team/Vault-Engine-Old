using System;
using Vault;

namespace Garbage
{
    public class Player : Entity
    {
        void OnStart()
        {
            SetObjectID();
        }

        void OnUpdate()
        {
            if (Input.IsKeyPressed(Input.KEY_Q)) GameObject.RemoveGameObject(ID);
        }

        // This function will get called if a mouse is over the GameObject
        void OnMouseEnter()
        {

        }

        // This function will get called if a mouse isn't over the GameObject anymore
        void OnMouseExit()
        {

        }

        void OnGUI()
        {

        }
    }
}