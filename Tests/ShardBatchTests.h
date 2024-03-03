#pragma once

#include "ReliquaryFixture.h"

#include "BasicShard.h"

using namespace Arca;

class ShardBatchFixture : public ReliquaryFixture
{
public:
    class UnregisteredShard;
    class GlobalRelic;
};

namespace Arca
{
    template<>
    struct Traits<ShardBatchFixture::UnregisteredShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ShardBatchTestsUnregisteredShard";
    };

    template<>
    struct Traits<ShardBatchFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ShardBatchTestsGlobalRelic";
        static const Locality locality = Locality::Global;
    };
}

class ShardBatchFixture::UnregisteredShard
{};

class ShardBatchFixture::GlobalRelic final
{
public:
    Index<BasicShard> shard;

    explicit GlobalRelic(RelicInit init);
};

namespace Inscription
{
    template<class Archive>
    struct ScribeTraits<ShardBatchFixture::GlobalRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<ShardBatchFixture::GlobalRelic>;
    };
}