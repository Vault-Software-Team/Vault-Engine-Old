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

            string url = "https://raw.githubusercontent.com/Vault-Software-Team/Vault-Engine/main/compile_flags.txt";
            HttpClient httpClient = new HttpClient();
            var result = httpClient.GetAsync(url).Result;
        }

        void OnUpdate()
        {
            // if (Input.IsKeyPressed(Input.KEY_Q)) GameObject.RemoveGameObject(ID);
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