#pragma once

#include "ReliquaryFixture.h"

#include <Arca/ClosedTypedRelic.h>

using namespace Arca;

class RelicBatchFixture : public ReliquaryFixture
{
public:
    class Relic;
    class GlobalRelic;
    class UnregisteredRelic;
    class Shard;
};

class RelicBatchFixture::Relic final : public ClosedTypedRelic<Relic>
{
public:
    int value = 0;
public:
    explicit Relic(Init init) : ClosedTypedRelic(init)
    {}
};

class RelicBatchFixture::GlobalRelic final : public ClosedTypedRelic<GlobalRelic>
{
public:
    int value = 0;
public:
    explicit GlobalRelic(Init init) : ClosedTypedRelic(init)
    {}
};

class RelicBatchFixture::UnregisteredRelic final : public ClosedTypedRelic<UnregisteredRelic>
{
public:
    explicit UnregisteredRelic(Init init) : ClosedTypedRelic(init)
    {}
};

class RelicBatchFixture::Shard
{
public:
    int value = 0;
public:
    Shard() = default;
    explicit Shard(int value);
};

namespace Arca
{
    template<>
    struct Traits<::RelicBatchFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicBatchTestsRelic";
    };

    template<>
    struct Traits<::RelicBatchFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicBatchTestsGlobalRelic";
    };

    template<>
    struct Traits<::RelicBatchFixture::UnregisteredRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicBatchTestsUnregisteredRelic";
    };

    template<>
    struct Traits<::RelicBatchFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "RelicBatchTestsShard";
    };
}

namespace Inscription
{
    template<>
    class Scribe<::RelicBatchFixture::Relic, BinaryArchive> final
        : public ArcaCompositeScribe<::RelicBatchFixture::Relic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.value);
        }
    };

    template<>
    class Scribe<::RelicBatchFixture::GlobalRelic, BinaryArchive> final
        : public ArcaCompositeScribe<::RelicBatchFixture::Relic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.value);
        }
    };

    template<>
    class Scribe<::RelicBatchFixture::Shard, BinaryArchive> final
        : public ArcaCompositeScribe<::RelicBatchFixture::Shard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.value);
        }
    };
}