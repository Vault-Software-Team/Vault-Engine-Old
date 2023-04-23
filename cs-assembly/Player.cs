using System;
using Vault;

namespace Sandbox
{
    public class Player : Entity
    {
        Transform transform;
        Entity parent;

        void OnStart()
        {
            SetObjectID();
            transform = GetComponent<Transform>();
            parent = GetEntity(GameObject.GetIDByTag("CubePoint"));
        }

        bool pressed = false;

        void OnUpdate()
        {
            if (Input.IsKeyPressed(Input.KEY_Q) && !pressed)
            {
                Debug.Log("Spawned!");
                GameObject.InstantiatePrefabWithProperties("assets/Cube.prefab", new Vector3(-4.56f, 2.31f, 0.0f), new Vector3(0, 0, 0.785f), parent.ID);
                pressed = true;
            }

            if (Input.IsKeyReleased(Input.KEY_Q) && pressed)
            {
                pressed = false;
            }
        }
    }
}
