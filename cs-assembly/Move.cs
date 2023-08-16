using System;
using Vault;

namespace Moving
{
    class Move : Entity
    {
        Transform transform;

        float force_speed = 8;
        bool done = false;
        int next_index;
        // Random random;

        Vector2[] positions = {
            new Vector2(9.26f, -2.72f),
            new Vector2(11.66f, 0.32f),
            new Vector2(10.16f, -1.92f),
        };

        void OnStart()
        {
            SetObjectID();
            transform = GetComponent<Transform>();

            // random = new Random();
        }

        void OnUpdate()
        {
            Vector3 pos = transform.position;
            if (pos == null) return;

            pos.x += force_speed * Main.deltaTime;
            transform.position = pos;

            if (pos.x >= Managers.GameManager.oEnd)
            {
                pos.x = Managers.GameManager.oStart;

                next_index++;
                if (next_index > 2)
                {
                    next_index = 0;
                }
                Vector2 new_y = positions[next_index];
                if (name.Contains("Top"))
                {
                    pos.y = new_y.x;
                    Managers.GameManager.score++;
                };
                if (name.Contains("Bottom")) pos.y = new_y.y;

                transform.position = pos;
            }

        }

        void OnGUI()
        {

        }
    }
}