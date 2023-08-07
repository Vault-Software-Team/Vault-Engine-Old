#include "Discord.hpp"
#include <iostream>

namespace DiscordAPI {
    DLL_API time_t timestamp = time(0);

    DLL_API void UpdatePresence(const std::string &details,
                                const std::string &state,
                                const std::string &largeImageKey,
                                const std::string &largeImageText,
                                const std::string &smallImageKey,
                                const std::string &smallImageText) {
        char buffer[256];
        DiscordRichPresence discordPresence;
        memset(&discordPresence, 0, sizeof(discordPresence));
        discordPresence.largeImageKey = largeImageKey.c_str();
        discordPresence.largeImageText = largeImageText.c_str();
        discordPresence.smallImageKey = smallImageKey.c_str();
        discordPresence.smallImageText = smallImageText.c_str();
        discordPresence.state = state.c_str();
        discordPresence.details = details.c_str();
        discordPresence.startTimestamp = timestamp;
        Discord_UpdatePresence(&discordPresence);
    }

    DLL_API void InitDiscordRPC(const std::string &id) {
        DiscordEventHandlers handlers;
        memset(&handlers, 0, sizeof(handlers));
        handlers.ready = [](const DiscordUser *request) {
            std::cout << "Discord: Ready" << std::endl;
        };

        handlers.errored = [](int errorCode, const char *message) {
            std::cout << "Discord: Error " << errorCode << ": " << message
                      << std::endl;
        };

        handlers.disconnected = [](int errorCode, const char *message) {
            std::cout << "Discord: Disconnected " << errorCode << ": " << message
                      << std::endl;
        };

        handlers.joinGame = [](const char *joinSecret) {
            std::cout << "Discord: Join Game " << joinSecret << std::endl;
        };

        handlers.spectateGame = [](const char *spectateSecret) {
            std::cout << "Discord: Spectate Game " << spectateSecret << std::endl;
        };

        Discord_Initialize(id.c_str(), &handlers, 1, nullptr);
    }
} // namespace DiscordAPI