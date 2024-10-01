#include "ila/event/server/ServerPongEvent.h"
#include "ila/iListenAttentively.h"
#include <mc/deps/raknet/SystemAddress.h>

namespace ila::inline server
{

struct RNS2_SendParameters
{
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
    int                  pRns2Socket,
    RNS2_SendParameters* pSendParameters,
    char const*          pFile,
    uint                 pLine
)
{
    if (pSendParameters->data[0] == 28)
    {
        constexpr static int head_size = sizeof(char) + sizeof(std::uint64_t) + sizeof(std::uint64_t) + 16;
        const char*          data      = pSendParameters->data;
        std::size_t          strlen    = data[head_size] << 8 | data[head_size + 1];
        if (strlen == 0) { return origin(pRns2Socket, pSendParameters, pFile, pLine); }
        std::istringstream       iss(std::string({ data + head_size + 2, strlen }));
        std::string              tmp;
        std::vector<std::string> parts;
        while (std::getline(iss, tmp, ';')) { parts.push_back(tmp); }
        if (parts.size() != 13) { return origin(pRns2Socket, pSendParameters, pFile, pLine); }

        std::string motd            = parts[1];
        int         protocolVersion = std::stoi(parts[2]);
        std::string networkVersion  = parts[3];
        int         playerCount     = std::stoi(parts[4]);
        int         maxPlayerCount  = std::stoi(parts[5]);
        std::string guid            = parts[6];
        std::string levelName       = parts[7];
        auto        gameType        = magic_enum::enum_cast<GameType>(parts[8]);

        if (!gameType.has_value()) { return origin(pRns2Socket, pSendParameters, pFile, pLine); }

        GameType mGameType    = gameType.value();
        ushort   mLoaclPort   = static_cast<ushort>(std::stoi(parts[10]));
        ushort   mLoaclPortV6 = static_cast<ushort>(std::stoi(parts[11]));

        auto event = ServerPongEvent(
            motd,
            protocolVersion,
            networkVersion,
            playerCount,
            maxPlayerCount,
            guid,
            levelName,
            mGameType,
            mLoaclPort,
            mLoaclPortV6
        );
        ll::event::EventBus::getInstance().publish(event);

        std::string result = fmt::format(
            "MCPE;{};{};{};{};{};{};{};{};1;{};{};0;",
            motd,
            protocolVersion,
            networkVersion,
            playerCount,
            maxPlayerCount,
            guid,
            levelName,
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
        pSendParameters->data   = packet.data();
        pSendParameters->length = static_cast<int>(packet.size());

        return origin(pRns2Socket, pSendParameters, pFile, pLine);
    }
    return origin(pRns2Socket, pSendParameters, pFile, pLine);
}

static std::unique_ptr<ll::event::EmitterBase> emitterFactory(ll::event::ListenerBase&);
class ServerPongEventEmitter : public ll::event::Emitter<emitterFactory, ServerPongEvent>
{
    ll::memory::HookRegistrar<ServerPongEventHook> hook;
};

static std::unique_ptr<ll::event::EmitterBase> emitterFactory(ll::event::ListenerBase&)
{
    return std::make_unique<ServerPongEventEmitter>();
}
} // namespace ila::inline server