
using System;
using Vault;

namespace NewScriptNamespace
{
    // DO NOT REMOVE THE FUNCTIONS FROM THIS SCRIPT!
    public class NewScript : Entity
    {
        // This gets called when the game/scene starts
        void OnStart()
        {
            // This function sets the scripts current object it is attached to, do not remove this!
            // Type your code below this function!
            SetObjectID();
            Debug.Log("Hello!");
        }

        // This gets called every frame
        void OnUpdate() {}

        // This function will get called if a mouse is over the GameObject
        void OnMouseEnter(){}

        // This function will get called if a mouse isn't over the GameObject anymore
        void OnMouseExit(){}

        void OnGUI() {}
    }
}
                             
