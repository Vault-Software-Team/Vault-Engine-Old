#include "3DTextFunctions.hpp"
#include "csharp.hpp"
#include <Discord/Discord.hpp>

namespace HyperAPI::CsharpScriptEngine::Functions {
    void Discord_SetPresence(MonoString *mono_details,
                             MonoString *mono_state,
                             MonoString *mono_largeImageKey,
                             MonoString *mono_largeImageText,
                             MonoString *mono_smallImageKey,
                             MonoString *mono_smallImageText) {
        std::string details = mono_string_to_utf8(mono_details);
        std::string state = mono_string_to_utf8(mono_state);
        std::string largeImageKey = mono_string_to_utf8(mono_largeImageKey);
        std::string largeImageText = mono_string_to_utf8(mono_largeImageText);
        std::string smallImageKey = mono_string_to_utf8(mono_smallImageKey);
        std::string smallImageText = mono_string_to_utf8(mono_smallImageText);

        DiscordAPI::UpdatePresence(details, state, largeImageKey, largeImageText, smallImageKey, smallImageText);
    }

    void Discord_InitRPC(MonoString *client_id) {
        std::string clientId = mono_string_to_utf8(client_id);
        DiscordAPI::InitDiscordRPC(clientId);
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions