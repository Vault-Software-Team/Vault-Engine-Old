using System;
using Vault;

namespace UI
{
    class ScoreText : Entity
    {
        Text3D text;

        void OnStart()
        {
            SetObjectID();

            text = GetComponent<Text3D>();
        }

        void OnUpdate()
        {
            // text.text = "Score: " + Managers.GameManager.score.ToString();
        }
    }
}