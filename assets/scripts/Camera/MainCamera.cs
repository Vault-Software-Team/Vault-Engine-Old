
using System;
using Vault;

namespace CameraScripts
{
    // DO NOT REMOVE THE FUNCTIONS FROM THIS SCRIPT!
    public class MainCamera : Entity
    {
        public Transform player;
        public CameraComponent camera;
        // offsets.x = x, and offsets.y = z
        private Vector2 offsets = new Vector2(0, 7.20f);
        private float lerp_speed = 3.0f;
        private float main_fov = 45.0f, zoomed_out_fov = 60.0f;
        public float current_fov = 45.0f;
        private float fov_lerp_speed = 5.0f;
        // This gets called when the game/scene starts

        void OnStart()
        {
            // This function sets the scripts current object it is attached to, do not remove this!
            // Type your code below this function!
            SetObjectID();
            player = GetEntity(GameObject.GetIDByTag("Player")).GetComponent<Transform>();
            camera = GetComponent<CameraComponent>();
        }

        // This gets called every frame
        void OnUpdate()
        {
            transform.position = new Vector3(
                Mathf.Lerp(transform.position.x, player.position.x + offsets.x, lerp_speed * Main.deltaTime),
                transform.position.y,
                Mathf.Lerp(transform.position.z, player.position.z + offsets.y, lerp_speed * Main.deltaTime)
            );

            camera.fov = Mathf.Lerp(camera.fov, current_fov, fov_lerp_speed * Main.deltaTime);

            if (Input.IsKeyPressed(Input.KEY_EQUAL))
            {
                current_fov = zoomed_out_fov;
            }
            else
            {
                current_fov = main_fov;
            }
        }

        // This function will get called if a mouse is over the GameObject
        void OnMouseEnter() { }

        // This function will get called if a mouse isn't over the GameObject anymore
        void OnMouseExit() { }

        void OnGUI() { }
    }
}