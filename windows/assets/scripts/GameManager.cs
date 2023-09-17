using System;
using Vault;

namespace Managers
{
    public class GameManager : Entity
    {
        public int score = 0;
        public static GameManager instance;

        void OnStart()
        {
            SetObjectID();
            instance = this;

            Discord.Init("1152212047015202817");
            Discord.SetPresence("Flapping thru hell", "Score: 0", "player", "ermmmm...", "", "");
        }

        void OnUpdate()
        {
            Discord.SetPresence("Flapping thru hell", "Score: " + score.ToString(), "player", "ermmmm...", "", "");
        }
    }
}