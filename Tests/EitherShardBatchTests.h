#pragma once

#include "ReliquaryFixture.h"

#include <Arca/Shard.h>
#include <Arca/TypedRelicAutomation.h>

using namespace Arca;

class EitherShardBatchFixture : public ReliquaryFixture
{
public:
    class Shard;
    class Relic;
};

class EitherShardBatchFixture::Shard
{
public:
    int value = 0;
public:
    Shard() = default;
    explicit Shard(int value);
};

class EitherShardBatchFixture::Relic : public TypedRelicAutomation<Relic, Shard>
{};

namespace Arca
{
    template<>
    struct Traits<::EitherShardBatchFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::EitherShardBatchFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::EitherShardBatchFixture::Shard, BinaryArchive> final
        : public ShardScribe<::EitherShardBatchFixture::Shard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.value);
        }
    };

    template<>
    class Scribe<::EitherShardBatchFixture::Relic, BinaryArchive> final
        : public CompositeRelicScribe<::EitherShardBatchFixture::Relic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };
}