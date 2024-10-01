#include "ServerPongEvent.h"

#include <mc/deps/raknet/SystemAddress.h>

namespace iListenAttentively::Level {

struct RNS2_SendParameters {
    char*                 data;
    int                   length;
    RakNet::SystemAddress system_address;
    int                   ttl;
};

LL_STATIC_HOOK(
    ServerPongEventHook,
    HookPriority::Normal,
    "?Send_Windows_Linux_360NoVDP@RNS2_Windows_Linux_360@RakNet@@KAHHPEAURNS2_SendParameters@2@PEBDI@Z",
    int,
    int                  rns2Socket,
    RNS2_SendParameters* sendParameters,
    char const*          file,
    uint                 line
) {
    if (sendParameters->data[0] == 28) {
        constexpr static int head_size = sizeof(char) + sizeof(std::uint64_t) + sizeof(std::uint64_t) + 16;
        const char*          data      = sendParameters->data;
        std::size_t          strlen    = data[head_size] << 8 | data[head_size + 1];
        if (strlen == 0) return origin(rns2Socket, sendParameters, file, line);
        std::istringstream       iss(std::string({data + head_size + 2, strlen}));
        std::string              tmp;
        std::vector<std::string> parts;
        while (std::getline(iss, tmp, ';')) parts.push_back(tmp);
        if (parts.size() != 13) return origin(rns2Socket, sendParameters, file, line);

        std::string mMotd            = parts[1];
        int         mProtocolVersion = std::stoi(parts[2]);
        std::string mNetworkVersion  = parts[3];
        int         mPlayerCount     = std::stoi(parts[4]);
        int         mMaxPlayerCount  = std::stoi(parts[5]);
        std::string mGuid            = parts[6];
        std::string mLevelName       = parts[7];
        auto        gameType         = magic_enum::enum_cast<GameType>(parts[8]);
        if (!gameType.has_value()) return origin(rns2Socket, sendParameters, file, line);
        GameType mGameType    = gameType.value();
        ushort   mLoaclPort   = static_cast<ushort>(std::stoi(parts[10]));
        ushort   mLoaclPortV6 = static_cast<ushort>(std::stoi(parts[11]));

        auto event = ServerPongEvent(
            mMotd,
            mProtocolVersion,
            mNetworkVersion,
            mPlayerCount,
            mMaxPlayerCount,
            mGuid,
            mLevelName,
            mGameType,
            mLoaclPort,
            mLoaclPortV6
        );
        ll::event::EventBus::getInstance().publish(event);

        std::string result = fmt::format(
            "MCPE;{};{};{};{};{};{};{};{};1;{};{};0;",
            mMotd,
            mProtocolVersion,
            mNetworkVersion,
            mPlayerCount,
            mMaxPlayerCount,
            mGuid,
            mLevelName,
            magic_enum::enum_name(mGameType),
            mLoaclPort,
            mLoaclPortV6
        );
        std::vector<char> packet;
        packet.reserve(256);
        packet.insert(packet.end(), data, data + head_size);
        strlen = result.length();
        packet.push_back(static_cast<char>((strlen >> 8) & 0xFF));
        packet.push_back(static_cast<char>(strlen & 0xFF));
        packet.insert(packet.end(), result.begin(), result.end());
        sendParameters->data   = packet.data();
        sendParameters->length = static_cast<int>(packet.size());
        return origin(rns2Socket, sendParameters, file, line);
    }
    return origin(rns2Socket, sendParameters, file, line);
}

static std::unique_ptr<ll::event::EmitterBase> emitterFactory(ll::event::ListenerBase&);
class ServerPongEventEmitter : public ll::event::Emitter<emitterFactory, ServerPongEvent> {
    ll::memory::HookRegistrar<ServerPongEventHook> hook;
};

static std::unique_ptr<ll::event::EmitterBase> emitterFactory(ll::event::ListenerBase&) {
    return std::make_unique<ServerPongEventEmitter>();
}
} // namespace iListenAttentively::Level