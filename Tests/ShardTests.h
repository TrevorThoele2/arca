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
};

namespace Arca
{
    template<>
    struct Traits<ShardTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ShardTestsShard";
    };
}

namespace Inscription
{
    template<>
    class Scribe<ShardTestsFixture::Shard, BinaryArchive> final :
        public ArcaNullScribe<ShardTestsFixture::Shard, BinaryArchive>
    {};
}