using System;
using Vault;

namespace FishingGearMemes
{
    public class FishingGear : Entity
    {
        SpriteRenderer renderer;
        public bool changed = false;

        List<string> fishing_gear_memes = new List<string>{
            "download.jpg",
            "meme2.png",
            "meme3.png",
            "meme4.png",
            "meme5.png",
        };

        void OnStart()
        {
            SetObjectID();

            renderer = GetComponent<SpriteRenderer>();
        }

        void OnUpdate()
        {
            int index = Main.RandomInt(0, fishing_gear_memes.Count - 1);

            if (Input.IsMouseButtonPressed(0) && !changed)
            {
                renderer.texture = "assets/textures/" + fishing_gear_memes[index];
                renderer.color = new Vector3(1, 1, 1);
                Audio.Play("assets/music/splash.mp3", 1, false);
                changed = true;
            }
        }
    }
}