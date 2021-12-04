#pragma once

#include "GeneralFixture.h"

#include <Arca/Shard.h>

#include "BasicShard.h"

using namespace Arca;

class ShardTestsFixture : public GeneralFixture
{
public:
    struct RelicConstructingShard
    {
        Index<BasicShard> shard;

        RelicConstructingShard(RelicInit init);
    };

    struct ShardConstructedFromMovedValue
    {
        std::unique_ptr<int> myInt;

        ShardConstructedFromMovedValue() = default;
        explicit ShardConstructedFromMovedValue(std::unique_ptr<int>&& myInt);
        ShardConstructedFromMovedValue(ShardConstructedFromMovedValue&& arg) = default;
    };
};

namespace Arca
{
    template<>
    struct Traits<ShardTestsFixture::RelicConstructingShard>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "RelicConstructingShard";
    };

    template<>
    struct Traits<ShardTestsFixture::ShardConstructedFromMovedValue>
    {
        static constexpr ObjectType objectType = ObjectType::Shard;
        static const inline TypeName typeName = "ShardTestsShardConstructedFromMovedValue";
    };
}

namespace Inscription
{
    template<class Archive>
    struct ScribeTraits<ShardTestsFixture::RelicConstructingShard, Archive> final
    {
        using Category = ArcaNullScribeCategory<ShardTestsFixture::RelicConstructingShard>;
    };

    template<class Archive>
    struct ScribeTraits<ShardTestsFixture::ShardConstructedFromMovedValue, Archive> final
    {
        using Category = ArcaNullScribeCategory<ShardTestsFixture::ShardConstructedFromMovedValue>;
    };
}