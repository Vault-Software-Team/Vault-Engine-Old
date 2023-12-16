using System;
using Vault;

namespace bla
{
    public class Enemy : Entity
    {
        Rigidbody2D rigidbody;
        Transform player_transform;
        float velocity = 0.5f;
        void OnStart()
        {
            SetObjectID();

            rigidbody = GetComponent<Rigidbody2D>();
            player_transform = GetEntity(GameObject.GetIDByName("Player")).GetComponent<Transform>();

        }

        void OnUpdate()
        {
            rigidbody.MoveToPosition(new Vector2(player_transform.position.x, player_transform.position.y), velocity);
        }

        // This function will get called if a mouse is over the GameObject
        void OnMouseEnter() { }

        // This function will get called if a mouse isn't over the GameObject anymore
        void OnMouseExit() { }

        void OnGUI() { }
    }
}