#pragma once

#include "GeneralFixture.h"

#include <Arca/Shard.h>

using namespace Arca;

class ShardTestsFixture : public GeneralFixture
{
public:
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
    struct Traits<ShardTestsFixture::ShardConstructedFromMovedValue>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static TypeName TypeName() { return "ShardTestsShardConstructedFromMovedValue"; }
    };
}

namespace Inscription
{
    template<class Archive>
    struct ScribeTraits<ShardTestsFixture::ShardConstructedFromMovedValue, Archive> final
    {
        using Category = ArcaNullScribeCategory<ShardTestsFixture::ShardConstructedFromMovedValue>;
    };
}