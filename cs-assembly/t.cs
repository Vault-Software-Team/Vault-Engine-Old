using System;
using Vault;

namespace Garbage
{
    public class Player : Entity
    {
        void OnStart()
        {
            SetObjectID();

            bla.Camera cam = GetEntity(GameObject.GetIDByName("Camera")).As<bla.Camera>();
            if (cam != null)
                Debug.Log(cam.deez);
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