using System;
using Vault;

namespace TextChanging
{
    public class ChangeText : Entity
    {
        Text3D text3D;

        bool pressed = false;
        bool is_happy = false;

        void OnStart()
        {
            // We set the entity data
            SetObjectID();

            // We get the Text3D component that the GameObject has
            text3D = GetComponent<Text3D>();
        }

        void OnUpdate()
        {
            // We check if we pressed the space key, if we did set is happy to the opposite of its current value
            if (Input.IsKeyPressed(Input.KEY_SPACE) && !pressed)
            {
                is_happy = !is_happy;
                pressed = true;
            }
            else if (Input.IsKeyReleased(Input.KEY_SPACE) && pressed)
            {
                // If the key was released then make the pressed go away
                pressed = false;
            }

            text3D.text = is_happy ? "I'm happy!" : "I'm sad!";
        }
    }
}