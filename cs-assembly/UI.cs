using System.Drawing;
using System;
using Vault;

namespace Namethiswhateveruwant
{

    // public and : Entity important!
    public class NameThisWhatevUWant : Entity
    {
        // PointLight light;
        // This gets called when a game starts
        void OnStart()
        {
            SetObjectID();

            // light = GetComponent<PointLight>();
        }

        // This gets called every frame
        void OnUpdate()
        {
            // light.color.x += Main.deltaTime;
            // light.color.y += Main.deltaTime;
            // light.color.z += Main.deltaTime;

            // if (light.color.x >= 1)
            // {
            //     light.color.x = 0;
            // }

            // if (light.color.y >= 1)
            // {
            //     light.color.y = 0;
            // }

            // if (light.color.z >= 1)
            // {
            //     light.color.z = 0;
            // }
        }

        void OnGUI()
        {

        }
    }
}