#pragma once

#include "ReliquaryFixture.h"

#include <Arca/ClosedTypedRelic.h>

using namespace Arca;

class EitherBatchTestsFixture : public ReliquaryFixture
{
public:
    class Shard;
    class Relic;
    class GlobalRelic;
};

class EitherBatchTestsFixture::Shard
{
public:
    int value = 0;
public:
    explicit Shard() = default;
    explicit Shard(int value);
};

class EitherBatchTestsFixture::Relic final : public ClosedTypedRelic<Relic>
{
public:
    explicit Relic(Init init);
};

class EitherBatchTestsFixture::GlobalRelic final : public ClosedTypedRelic<GlobalRelic>
{
public:
    explicit GlobalRelic(Init init);
};

namespace Arca
{
    template<>
    struct Traits<::EitherBatchTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "EitherShardBatchTestsShard";
    };

    template<>
    struct Traits<::EitherBatchTestsFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "EitherShardBatchTestsRelic";
    };

    template<>
    struct Traits<::EitherBatchTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "EitherShardBatchTestsGlobalRelic";
        static const Locality locality = Locality::Global;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::EitherBatchTestsFixture::Shard, BinaryArchive> final
        : public ArcaCompositeScribe<::EitherBatchTestsFixture::Shard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.value);
        }
    };

    template<>
    class Scribe<::EitherBatchTestsFixture::Relic, BinaryArchive> final
        : public ArcaNullScribe<::EitherBatchTestsFixture::Relic, BinaryArchive>
    {};

    template<>
    class Scribe<::EitherBatchTestsFixture::GlobalRelic, BinaryArchive> final
        : public ArcaNullScribe<::EitherBatchTestsFixture::GlobalRelic, BinaryArchive>
    {};
}