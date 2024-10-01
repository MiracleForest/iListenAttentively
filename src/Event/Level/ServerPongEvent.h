#include "Global.h"

#include <mc/enums/GameType.h>

namespace iListenAttentively::Level {
class ServerPongEvent final : public ll::event::Event {
protected:
    std::string& mMotd;
    int&         mProtocolVersion;
    std::string& mNetworkVersion;
    int&         mPlayerCount;
    int&         mMaxPlayerCount;
    std::string& mGuid;
    std::string& mLevelName;
    GameType&    mGameMode;
    ushort&      mLoaclPort;
    ushort&      mLoaclPortV6;

public:
    constexpr explicit ServerPongEvent(
        std::string& motd,
        int&         protocolVersion,
        std::string& networkVersion,
        int&         playerCount,
        int&         maxPlayerCount,
        std::string& guid,
        std::string& levelName,
        GameType&    gameMode,
        ushort&      localPort,
        ushort&      localPortV6
    )
    : mMotd(motd),
      mProtocolVersion(protocolVersion),
      mNetworkVersion(networkVersion),
      mPlayerCount(playerCount),
      mMaxPlayerCount(maxPlayerCount),
      mGuid(guid),
      mLevelName(levelName),
      mGameMode(gameMode),
      mLoaclPort(localPort),
      mLoaclPortV6(localPortV6) {}

    std::string& getMotd() const { return mMotd; }
    int&         getProtocolVersion() const { return mProtocolVersion; }
    std::string& getNetworkVersion() const { return mNetworkVersion; }
    int&         getPlayerCount() const { return mPlayerCount; }
    int&         getMaxPlayerCount() const { return mMaxPlayerCount; }
    std::string& getGuid() const { return mGuid; }
    std::string& getLevelName() const { return mLevelName; }
    GameType&    getGameMode() const { return mGameMode; }
    ushort&      getLocalPort() const { return mLoaclPort; }
    ushort&      getLocalPortV6() const { return mLoaclPortV6; }
};
} // namespace iListenAttentively::Level