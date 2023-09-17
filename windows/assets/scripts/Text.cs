using System;
using Vault;

namespace TextScripts
{
    public class ScoreText : Entity
    {
        Text3D text3D;

        void OnStart()
        {
            SetObjectID();
            text3D = GetComponent<Text3D>();
        }

        void OnUpdate()
        {
            text3D.text = "Score: " + Managers.GameManager.instance.score.ToString();
        }
    }
}