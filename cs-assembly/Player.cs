using System.Reflection.Metadata;
using System;
using Vault;

namespace PlayerScripts
{
    class Player : Entity
    {
        Rigidbody3D rigidbody;
        Transform transform;

        float force_speed = 20;
        float gravity = 7;
        float pressed_y = 0;

        bool already_pressed = false;
        bool stopGravity = false;

        // Rotations
        float on_force_rotation = Main.ToRad(40);
        float on_fall_rotation = Main.ToRad(125);
        float rot_speed = 5;

        void OnStart()
        {
            SetObjectID();
            rigidbody = GetComponent<Rigidbody3D>();
            transform = GetComponent<Transform>();
            Debug.Log("Player Script Started!");
        }

        void OnUpdate()
        {
            Vector3 pos = transform.position;
            if (pos == null) return;
            Vector3 rot = transform.rotation;
            if ((Input.IsKeyPressed(Input.KEY_SPACE) || Input.IsKeyPressed(Input.KEY_W)) && (!already_pressed && !stopGravity))
            {
                pressed_y = pos.y;
                already_pressed = true;
                stopGravity = true;
            }
            else if (Input.IsKeyReleased(Input.KEY_SPACE) && Input.IsKeyReleased(Input.KEY_W))
            {
                already_pressed = false;
            }

            if (!stopGravity)
            {
                pos.y -= gravity * Main.deltaTime;
            }
            else
            {
                pos.y = Lerp.Float(pos.y, pressed_y + 1.2f, force_speed * Main.deltaTime);

                if (pos.y > pressed_y + 1.18f)
                {
                    pressed_y = 0;
                    stopGravity = false;
                }
            }

            if (pos.y < 0.43f)
            {
                pos.y = 0.43f;
            }

            transform.position = pos;
            transform.rotation = rot;
            rigidbody.SetPosition(pos);
        }

        void OnGUI()
        {

        }

        void OnCollisionEnter3D(string ID)
        {
            Debug.Log(ID);
        }
    }
}