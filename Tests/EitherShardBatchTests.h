#pragma once

#include "ReliquaryFixture.h"

#include <Arca/Shard.h>
#include <Arca/ClosedTypedRelicAutomation.h>

using namespace Arca;

class EitherShardBatchFixture : public ReliquaryFixture
{
public:
    class Shard;
    class Relic;
    class GlobalRelic;
};

class EitherShardBatchFixture::Shard
{
public:
    int value = 0;
public:
    Shard() = default;
    explicit Shard(int value);
};

class EitherShardBatchFixture::Relic : public ClosedTypedRelicAutomation<Relic, Shard>
{};

class EitherShardBatchFixture::GlobalRelic : public ClosedTypedRelicAutomation<GlobalRelic, Shard>
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

    template<>
    struct Traits<::EitherShardBatchFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
        static const Locality locality = Locality::Global;
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

    template<>
    class Scribe<::EitherShardBatchFixture::GlobalRelic, BinaryArchive> final
        : public CompositeRelicScribe<::EitherShardBatchFixture::GlobalRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };
}