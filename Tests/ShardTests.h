#pragma once

#include "GeneralFixture.h"

#include <Arca/Shard.h>

using namespace Arca;

class ShardTestsFixture : public GeneralFixture
{
public:
    struct Shard
    {
        int myInt;
    };

    template<size_t i>
    struct DifferentiableShard
    {
        int myInt;
    };

    struct ShardConstructedFromMovedValue
    {
        std::unique_ptr<int> myInt;

        ShardConstructedFromMovedValue() = default;
        explicit ShardConstructedFromMovedValue(std::unique_ptr<int>&& myInt);
    };
};

namespace Arca
{
    template<>
    struct Traits<ShardTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ShardTestsShard";
    };

    template<size_t i>
    struct Traits<ShardTestsFixture::DifferentiableShard<i>>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ShardTestsDifferentiableShard" + Chroma::ToString(i);
    };

    template<>
    struct Traits<ShardTestsFixture::ShardConstructedFromMovedValue>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ShardTestsShardConstructedFromMovedValue";
    };
}

namespace Inscription
{
    template<>
    class Scribe<ShardTestsFixture::Shard, BinaryArchive> final :
        public ArcaNullScribe<ShardTestsFixture::Shard, BinaryArchive>
    {};

    template<size_t i>
    class Scribe<ShardTestsFixture::DifferentiableShard<i>, BinaryArchive> final :
        public ArcaNullScribe<ShardTestsFixture::DifferentiableShard<i>, BinaryArchive>
    {};

    template<>
    class Scribe<ShardTestsFixture::ShardConstructedFromMovedValue, BinaryArchive> final :
        public ArcaNullScribe<ShardTestsFixture::ShardConstructedFromMovedValue, BinaryArchive>
    {};
}