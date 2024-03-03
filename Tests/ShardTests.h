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
}