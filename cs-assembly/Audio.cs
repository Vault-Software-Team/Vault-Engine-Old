using Vault;

namespace Sandbox
{
    public class AudioTest : Entity
    {
        Audio3D audio;

        void OnStart()
        {
            SetObjectID();

            // audio = GetComponent<Audio3D>();
            // audio.pitch = 1.5f;
        }

        void OnUpdate(float ts)
        {

        }
    }
}