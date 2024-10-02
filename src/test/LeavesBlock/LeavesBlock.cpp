#include <ll/api/memory/Hook.h>
#include <ll/api/memory/Memory.h>
#include <mc/math/Random.h>
#include <mc/world/Facing.h>
#include <mc/world/level/ActorBlockSyncMessage.h>
#include <mc/world/level/BlockPos.h>
#include <mc/world/level/BlockSource.h>
#include <mc/world/level/Level.h>
#include <mc/world/level/Randomize.h>
#include <mc/world/level/block/Block.h>
#include <mc/world/level/block/BlockLegacy.h>
#include <mc/world/level/block/LeavesBlock.h>
#include <mc/world/level/block/VanillaStates.h>
#include <mc/world/level/block/registry/BlockTypeRegistry.h>
#include <mc/world/level/block/states/BlockState.h>
#include <mc/world/level/block/utils/BedrockBlockNames.h>
#include <mc/world/level/block/utils/ResourceDropsContext.h>
#include <mc/world/level/block/utils/VanillaBlockTypeGroups.h>
#include <mc/world/level/block/utils/VanillaBlockTypeIds.h>
#include <mc/world/level/block/utils/VanillaBlockTypes.h>
#include <mc/world/phys/AABB.h>

LL_AUTO_TYPE_INSTANCE_HOOK(
    LeavesBlock_randomTickHook,
    ll::memory::HookPriority::Normal,
    LeavesBlock,
    "?randomTick@LeavesBlock@@UEBAXAEAVBlockSource@@AEBVBlockPos@@AEAVRandom@@@Z",
    void,
    BlockSource&    region,
    BlockPos const& pos,
    Random&         random
)
{
    std::array<int, 32768> checkBuffer {};

    Level& level = region.getLevel();


    Block* leafBlock = const_cast<Block*>(&region.getBlock(pos));

    bool bUpdateBit =
        LL_SYMBOL_CALL("??$getState@_N@Block@@QEBA_NAEBVBlockState@@@Z", bool, Block*, BlockState const&)(
            leafBlock,
            VanillaStates::UpdateBit
        );
    bool bPersistentBit =
        LL_SYMBOL_CALL("??$getState@_N@Block@@QEBA_NAEBVBlockState@@@Z", bool, Block*, BlockState const&)(
            leafBlock,
            VanillaStates::PersistentBit
        );
    AABB bb;
    if (bUpdateBit && !bPersistentBit)
    {
        Vec3 vPos = pos;
        AABB aabb(vPos, vPos);
        Vec3 distance(5);
        bb = aabb.cloneAndGrow(distance);

        if (region.hasChunksAt(bb, 0))
        {
            for (int xo = -4; xo <= 4; ++xo)
            {
                for (int yo = -4; yo <= 4; ++yo)
                {
                    for (int zo = -4; zo <= 4; ++zo)
                    {
                        Block* block =
                            const_cast<Block*>(&region.getBlock(xo + pos.x, yo + pos.y, zo + pos.z));
                        BlockLegacy const& legacyBlock = block->getLegacyBlock();
                        if (legacyBlock.anyOf(VanillaBlockTypeGroups::LogBlockIds)
                            || legacyBlock.anyOf(VanillaBlockTypeGroups::OldWoodAndStrippedWoodBlockIds)
                            || (ll::memory::dAccess<BlockLegacy::NameInfo>(
                                    &*VanillaBlockTypes::mMangroveWood,
                                    0x48
                                )
                                    .mFullName
                                == ll::memory::dAccess<BlockLegacy::NameInfo>(&legacyBlock, 0x48).mFullName)
                            || (VanillaBlockTypeIds::StrippedMangroveLog
                                == ll::memory::dAccess<BlockLegacy::NameInfo>(&legacyBlock, 0x48).mFullName)
                            || (ll::memory::dAccess<BlockLegacy::NameInfo>(
                                    &*VanillaBlockTypes::mStrippedMangroveWood,
                                    0x48
                                )
                                    .mFullName
                                == ll::memory::dAccess<BlockLegacy::NameInfo>(&legacyBlock, 0x48).mFullName)
                            || (VanillaBlockTypeIds::CherryLog
                                == ll::memory::dAccess<BlockLegacy::NameInfo>(&legacyBlock, 0x48).mFullName)
                            || (VanillaBlockTypeIds::CherryWood
                                == ll::memory::dAccess<BlockLegacy::NameInfo>(&legacyBlock, 0x48).mFullName)
                            || (VanillaBlockTypeIds::StrippedCherryLog
                                == ll::memory::dAccess<BlockLegacy::NameInfo>(&legacyBlock, 0x48).mFullName)
                            || (VanillaBlockTypeIds::StrippedCherryWood
                                == ll::memory::dAccess<BlockLegacy::NameInfo>(&legacyBlock, 0x48).mFullName))
                        {
                            checkBuffer[zo + 16 + 32 * (yo + 16) + ((xo + 16) << 10)] = 0;
                        }
                        else if (block->hasProperty(BlockProperty::Leaf))
                        {
                            checkBuffer[zo + 16 + 32 * (yo + 16) + ((xo + 16) << 10)] = -2;
                        }
                        else { checkBuffer[zo + 16 + 32 * (yo + 16) + ((xo + 16) << 10)] = -1; }
                    }
                }
            }
            for (int i = 1; i <= 4; ++i)
            {
                for (int xo_0 = -4; xo_0 <= 4; ++xo_0)
                {
                    for (int yo_0 = -4; yo_0 <= 4; ++yo_0)
                    {
                        for (int zo_0 = -4; zo_0 <= 4; ++zo_0)
                        {
                            if (checkBuffer[zo_0 + 16 + 32 * (yo_0 + 16) + ((xo_0 + 16) << 10)] == i - 1)
                            {
                                if (checkBuffer[zo_0 + 16 + 32 * (yo_0 + 16) + ((xo_0 + 15) << 10)] == -2)
                                    checkBuffer[zo_0 + 16 + 32 * (yo_0 + 16) + ((xo_0 + 15) << 10)] = i;
                                if (checkBuffer[zo_0 + 16 + 32 * (yo_0 + 16) + ((xo_0 + 17) << 10)] == -2)
                                    checkBuffer[zo_0 + 16 + 32 * (yo_0 + 16) + ((xo_0 + 17) << 10)] = i;
                                if (checkBuffer[zo_0 + 16 + 32 * (yo_0 + 15) + ((xo_0 + 16) << 10)] == -2)
                                    checkBuffer[zo_0 + 16 + 32 * (yo_0 + 15) + ((xo_0 + 16) << 10)] = i;
                                if (checkBuffer[zo_0 + 16 + 32 * (yo_0 + 17) + ((xo_0 + 16) << 10)] == -2)
                                    checkBuffer[zo_0 + 16 + 32 * (yo_0 + 17) + ((xo_0 + 16) << 10)] = i;
                                if (checkBuffer[zo_0 + 15 + 32 * (yo_0 + 16) + ((xo_0 + 16) << 10)] == -2)
                                    checkBuffer[zo_0 + 15 + 32 * (yo_0 + 16) + ((xo_0 + 16) << 10)] = i;
                                if (checkBuffer[zo_0 + 17 + 32 * (yo_0 + 16) + ((xo_0 + 16) << 10)] == -2)
                                    checkBuffer[zo_0 + 17 + 32 * (yo_0 + 16) + ((xo_0 + 16) << 10)] = i;
                            }
                        }
                    }
                }
            }
        }
        if (checkBuffer[0x4210uLL] < 0)
        {
            Random&   random = (Random&)level.getIRandom();
            Randomize randomize(random);
            region.getBlock(pos)
                .spawnResources(region, pos, randomize, ResourceDropsContext::fromOtherCause(region, pos));
            region.setBlock(
                pos,
                BlockTypeRegistry::getDefaultBlockState(BedrockBlockNames::Air, 1),
                3,
                0i64,
                0i64
            );
        }
        else
        {
            gsl::strict_not_null<Block const*> fblock =
                LL_SYMBOL_CALL("??$setState@W4Name@Facing@@@Block@@QEBA?AV?$strict_not_null@PEBVBlock@@@gsl@@AEBVBlockState@@W4Name@Facing@@@Z", gsl::strict_not_null<Block const*>, Block*, BlockState const&, Facing::Name)(
                    leafBlock,
                    VanillaStates::UpdateBit,
                    Facing::Name::Down
                );

            region.setBlock(pos.x, pos.y, pos.z, *fblock, 4, 0i64);
        }
    }
}
