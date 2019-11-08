#pragma once

#include "ReliquaryFixture.h"

#include <Arca/Shard.h>
#include <Arca/TypedRelicWithShards.h>

using namespace Arca;

class ShardBatchFixture : public ReliquaryFixture
{
public:
    class Shard;
    class UnregisteredShard;
    class StaticRelic;
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

class ShardBatchFixture::StaticRelic : public TypedRelicWithShards<Shard>
{
public:
    Shard* shard = nullptr;
public:
    StaticRelic() = default;
private:
    void Setup();
};

namespace Arca
{
    template<>
    struct ShardTraits<::ShardBatchFixture::Shard>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct ShardTraits<::ShardBatchFixture::UnregisteredShard>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::ShardBatchFixture::StaticRelic>
    {
        static const TypeHandle typeHandle;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::ShardBatchFixture::Shard, BinaryArchive> final
        : public ShardScribe<::ShardBatchFixture::Shard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.value);
        }
    };

    template<>
    class Scribe<::ShardBatchFixture::StaticRelic, BinaryArchive> final
        : public ShardScribe<::ShardBatchFixture::StaticRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.shard);
        }
    };
}