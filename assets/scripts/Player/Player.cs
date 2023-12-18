
using System;
using Vault;

namespace PlayerScripts
{
    // DO NOT REMOVE THE FUNCTIONS FROM THIS SCRIPT!
    public class Player : Entity
    {
        public CameraComponent camera;
        public Transform camera_transform;
        public Rigidbody3D rigidbody;
        public Transform sun_transform;
        private float camera_lerp_speed = 5.0f;
        private float cam_offset_y = 6.30f;
        private float cam_offset_z = 8.10f;

        private float movement_speed = 1000.0f;

        void OnStart()
        {
            SetObjectID();

            rigidbody = GetComponent<Rigidbody3D>();

            camera = GetEntity(GameObject.GetIDByTag("MainCamera")).GetComponent<CameraComponent>();
            camera_transform = GetEntity(GameObject.GetIDByTag("MainCamera")).GetComponent<Transform>();
            sun_transform = GetEntity(GameObject.GetIDByTag("Sun")).GetComponent<Transform>();
        }

        // This gets called every frame
        void OnUpdate()
        {
            camera_transform.position = new Vector3(
                Mathf.Lerp(camera_transform.position.x, transform.position.x, camera_lerp_speed * Main.deltaTime),
                Mathf.Lerp(camera_transform.position.y, transform.position.y + cam_offset_y, camera_lerp_speed * Main.deltaTime),
                Mathf.Lerp(camera_transform.position.z, transform.position.z + cam_offset_z, camera_lerp_speed * Main.deltaTime)
            );

            if (sun_transform.position.y <= 2.90f)
            {
                Main.ambient = Mathf.Lerp(Main.ambient, 0.02f, 2.5f * Main.deltaTime);
            }
            else
            {
                Main.ambient = Mathf.Lerp(Main.ambient, 0.2f, 2.5f * Main.deltaTime);
            }


            Movement();
            DayLight();
        }

        bool go_to_day = false;
        float day_pos = 20.0f, night_pos = -20.0f, daylight_lerp_speed = 0.02f;
        void DayLight()
        {
            if (go_to_day)
            {
                sun_transform.position = new Vector3(
                    sun_transform.position.x,
                    Mathf.Lerp(sun_transform.position.y, day_pos, daylight_lerp_speed * Main.deltaTime),
                    sun_transform.position.z
                );

                if (sun_transform.position.y > day_pos - 0.05f) go_to_day = false;
            }
            else
            {
                sun_transform.position = new Vector3(
                    sun_transform.position.x,
                    Mathf.Lerp(sun_transform.position.y, night_pos, daylight_lerp_speed * Main.deltaTime),
                    sun_transform.position.z
                );

                if (sun_transform.position.y < night_pos + 0.05f) go_to_day = true;
            }
        }

        void Movement()
        {
            int x = Input.GetHorizontalAxis();
            int y = Input.GetVerticalAxis();

            if (x != 0 || y != 0)
            {
                rigidbody.SetVelocity(new Vector3(x * movement_speed * Main.deltaTime, rigidbody.velocity.y, -(y * movement_speed) * Main.deltaTime));
            }
        }

        // This function will get called if a mouse is over the GameObject
        void OnMouseEnter() { }

        // This function will get called if a mouse isn't over the GameObject anymore
        void OnMouseExit() { }

        void OnGUI() { }
    }
}