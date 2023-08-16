using System;
using Vault;

namespace Managers
{
    public class GameManager : Entity
    {
        public static float oEnd = 6.47f;
        public static float oStart = -13.83f;
        public static int score = 0;

        void OnStart()
        {
            SetObjectID();
        }

        void OnUpdate()
        {

        }

        void OnGUI()
        {

        }
    }
}