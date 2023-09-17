using System;
using Vault;

namespace ObstacleScripts
{
    public class Obstacle : Entity
    {
        Rigidbody2D rigidbody;
        BoxCollider2D collider;

        float move_speed = 3.5f;
        float starting_position = 3.81f;
        float ending_position = -3.69f;

        int index = 0;

        List<Vector2> positions = new List<Vector2>{
            new Vector2(1.67f, -2.18f),
            new Vector2(1.23f, -2.53f),
            new Vector2(2.33f, -1.21f),
        };

        void OnStart()
        {
            SetObjectID();
            rigidbody = GetComponent<Rigidbody2D>();
            collider = GetComponent<BoxCollider2D>();
        }

        void OnUpdate()
        {

            rigidbody.SetVelocity(-move_speed, 0);
            if (transform.position.x <= ending_position)
            {
                if (name.Contains("Bottom"))
                {
                    transform.position = new Vector3(starting_position, positions[index].y, transform.position.z);
                }
                else if (name.Contains("Top"))
                {
                    transform.position = new Vector3(starting_position, positions[index].x, transform.position.z);
                    Managers.GameManager.instance.score++;
                }

                index++;
                if (index >= positions.Count) index = 0;
            }
        }
    }
}