#pragma once

#include "ReliquaryFixture.h"

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

class EitherShardBatchFixture::Relic final : public ClosedTypedRelicAutomation<Relic, Shard>
{};

class EitherShardBatchFixture::GlobalRelic final : public ClosedTypedRelicAutomation<GlobalRelic, Shard>
{};

namespace Arca
{
    template<>
    struct Traits<::EitherShardBatchFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::EitherShardBatchFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::EitherShardBatchFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
        static const Locality locality = Locality::Global;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::EitherShardBatchFixture::Shard, BinaryArchive> final
        : public ArcaCompositeScribe<::EitherShardBatchFixture::Shard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.value);
        }
    };

    template<>
    class Scribe<::EitherShardBatchFixture::Relic, BinaryArchive> final
        : public ArcaNullScribe<::EitherShardBatchFixture::Relic, BinaryArchive>
    {};

    template<>
    class Scribe<::EitherShardBatchFixture::GlobalRelic, BinaryArchive> final
        : public ArcaNullScribe<::EitherShardBatchFixture::GlobalRelic, BinaryArchive>
    {};
}