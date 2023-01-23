using System;
using Vault;

namespace Sandbox
{
    public class Player : Entity
    {
        private float jump_force = 50;
        private float speed = 1.5f;

        Rigidbody2D rigidbody;
        BoxCollider2D collider;

        void OnStart()
        {
            SetObjectID();
            
            rigidbody = GetComponent<Rigidbody2D>();
            collider = GetComponent<BoxCollider2D>();
        }

        private bool jumped = false;
        void OnUpdate(float ts)
        {  
            if(Input.IsKeyPressed(Input.KEY_SPACE) && !jumped && rigidbody.velocity.y == 0) {
                rigidbody.Force(0, jump_force);
                jumped = true;
            } else if(Input.IsKeyReleased(Input.KEY_SPACE) && jumped) {
                jumped = false;
            } 

            if(Input.IsKeyPressed(Input.KEY_A)) {
                rigidbody.SetVelocity(-speed, rigidbody.velocity.y);
            } 

            if(Input.IsKeyPressed(Input.KEY_D)) {
                rigidbody.SetVelocity(speed, rigidbody.velocity.y);
            } 
        }
    }
}
