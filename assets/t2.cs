using System;
using Vault;
using System.Net.Http;

namespace Garbage
{
    public class Player : Entity
    {
        void OnStart()
        {
            SetObjectID();
        }

        bool key_pressed = false, key_pressed2;
        void OnUpdate()
        {
            if (Input.IsKeyPressed(Input.KEY_E) && !key_pressed2)
            {
                GameObject.InstantiatePrefab("assets/Test.prefab");
                key_pressed2 = true;
            }
            else if (Input.IsKeyReleased(Input.KEY_E))
            {
                key_pressed2 = false;
            }

            if (Input.IsKeyPressed(Input.KEY_Q) && !key_pressed)
            {
                GameObject.RemoveGameObject(GameObject.GetIDByName("Prefab"));
                GameObject.RemoveGameObject(GameObject.GetIDByTag("Test"));
                key_pressed = true;
            }
            else if (Input.IsKeyReleased(Input.KEY_Q))
            {
                key_pressed = false;
            }
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