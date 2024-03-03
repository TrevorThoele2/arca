#pragma once

#include "ReliquaryFixture.h"

#include <Arca/ClosedTypedRelic.h>

using namespace Arca;

class ShardBatchFixture : public ReliquaryFixture
{
public:
    class Shard;
    class UnregisteredShard;
    class GlobalRelic;
};

class ShardBatchFixture::Shard
{
public:
    int value = 0;
public:
    Shard() = default;
    explicit Shard(int value);
};

class ShardBatchFixture::UnregisteredShard
{};

class ShardBatchFixture::GlobalRelic final : public ClosedTypedRelic<GlobalRelic>
{
public:
    ShardIndex<Shard> shard;

    explicit GlobalRelic(Initialization initialization);
};

namespace Arca
{
    template<>
    struct Traits<::ShardBatchFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ShardBatchTestsShard";
    };

    template<>
    struct Traits<::ShardBatchFixture::UnregisteredShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ShardBatchTestsUnregisteredShard";
    };

    template<>
    struct Traits<::ShardBatchFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ShardBatchTestsGlobalRelic";
        static const Locality locality = Locality::Global;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::ShardBatchFixture::Shard, BinaryArchive> final
        : public ArcaCompositeScribe<::ShardBatchFixture::Shard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.value);
        }
    };

    template<>
    class Scribe<::ShardBatchFixture::GlobalRelic, BinaryArchive> final
        : public ArcaNullScribe<::ShardBatchFixture::GlobalRelic, BinaryArchive>
    {};
}