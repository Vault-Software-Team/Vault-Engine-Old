using System;
using Vault;

namespace Sandbox
{
    public class Player : Entity
    {
        Text3D text;

        void OnStart()
        {
            SetObjectID();

            text = GetComponent<Text3D>();
            text.text = "Hello World!";
        }

        void OnUpdate(float ts)
        {

        }
    }
}
