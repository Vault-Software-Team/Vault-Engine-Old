using System;
using Vault;

namespace bla
{
    public class Camera : Entity
    {
        public string deez = "yo bro!";

        void OnStart()
        {
            SetObjectID();
        }

        void OnUpdate()
        {
            Main.ambient = 0.2f;
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