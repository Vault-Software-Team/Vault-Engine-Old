using System;
using Vault;

namespace PlayerScripts
{
    public class Player : Entity
    {
        Rigidbody2D rigidbody;
        BoxCollider2D collider;

        float jump_speed = 125;
        bool jumped = false;

        void OnStart()
        {
            SetObjectID();
            rigidbody = GetComponent<Rigidbody2D>();
            collider = GetComponent<BoxCollider2D>();
        }

        void OnUpdate()
        {
            bool press_check = Input.IsKeyPressed(Input.KEY_W) || Input.IsKeyPressed(Input.KEY_SPACE) || Input.IsMouseButtonPressed(0);
            bool press_release = Input.IsKeyReleased(Input.KEY_W) && Input.IsKeyReleased(Input.KEY_SPACE) && Input.IsMouseButtonReleased(0);

            if (press_check && !jumped)
            {
                rigidbody.Force(0, jump_speed);
                jumped = true;
            }
            else if (press_release)
            {
                jumped = false;
            }

            if (transform.position.y < -2.01 || transform.position.y > 2.29)
            {
                Scene.LoadScene("assets/scenes/main.vault");
            }
        }

        void OnCollisionEnter2D(string ID)
        {
            Entity entity = GetEntity(ID);
            if (entity.tag == "obstacle")
            {
                Scene.LoadScene("assets/scenes/main.vault");
            }
        }
    }
}