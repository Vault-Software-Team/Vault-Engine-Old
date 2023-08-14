#pragma once
#include <cstring>
#include <ctime>
#include <string>
#include <discord-rpc/discord_rpc.h>
#include <dllapi.hpp>

namespace DiscordAPI {
    DLL_API extern time_t timestamp;

    DLL_API void UpdatePresence(const std::string &details = "",
                                const std::string &state = "",
                                const std::string &largeImageKey = "logo",
                                const std::string &largeImageText = "",
                                const std::string &smallImageKey = "",
                                const std::string &smallImageText = "");

    DLL_API void InitDiscordRPC(const std::string &id);
} // namespace DiscordAPI