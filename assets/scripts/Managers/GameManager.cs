
using System;
using Vault;

namespace Managers
{
    // DO NOT REMOVE THE FUNCTIONS FROM THIS SCRIPT!
    public class GameManager : Entity
    {
        public static GameManager instance;

        void OnStart()
        {
            SetObjectID();
            instance = this;
        }

        void OnUpdate()
        {

        }

        void OnMouseEnter() { }
        void OnMouseExit() { }

        void OnGUI() { }
    }
}