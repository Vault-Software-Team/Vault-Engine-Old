
using System;
using Vault;

namespace PlayerScripts
{
    // DO NOT REMOVE THE FUNCTIONS FROM THIS SCRIPT!
    public class Player : Entity
    {
        public Rigidbody3D rigidbody;
        public CameraComponent camera;
        public Transform camera_transform;

        void OnStart()
        {
            SetObjectID();
            rigidbody = GetComponent<Rigidbody3D>();
            camera = GetEntity(GameObject.GetIDByTag("MainCamera")).GetComponent<CameraComponent>();
            camera_transform = GetEntity(GameObject.GetIDByTag("MainCamera")).GetComponent<Transform>();
            camera.mouse_movement = true;
        }

        // This gets called every frame
        void OnUpdate()
        {

        }

        // This function will get called if a mouse is over the GameObject
        void OnMouseEnter() { }

        // This function will get called if a mouse isn't over the GameObject anymore
        void OnMouseExit() { }

        void OnGUI() { }
    }
}