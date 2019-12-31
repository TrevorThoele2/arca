#pragma once

#include "ReliquaryFixture.h"

#include <Arca/ClosedTypedRelicAutomation.h>

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

class ShardBatchFixture::GlobalRelic final : public ClosedTypedRelicAutomation<GlobalRelic, Shard>
{
public:
    LocalPtr<Shard> shard;

    void PostConstruct(ShardTuple shards);
};

namespace Arca
{
    template<>
    struct Traits<::ShardBatchFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ShardBatchFixture::UnregisteredShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ShardBatchFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
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