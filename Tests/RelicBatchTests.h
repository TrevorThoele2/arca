#pragma once

#include "ReliquaryFixture.h"

#include <Arca/ShardTraits.h>
#include <Arca/ClosedTypedRelicAutomation.h>

using namespace Arca;

class RelicBatchFixture : public ReliquaryFixture
{
public:
    class Relic;
    class GlobalRelic;
    class UnregisteredRelic;
    class Shard;
};

class RelicBatchFixture::Relic : public ClosedTypedRelicAutomation<Relic>
{
public:
    int value = 0;
public:
    Relic() = default;
protected:
    void InitializeImplementation() override;
};

class RelicBatchFixture::GlobalRelic : public ClosedTypedRelicAutomation<GlobalRelic>
{
public:
    int value = 0;
public:
    GlobalRelic() = default;
protected:
    void InitializeImplementation() override;
};

class RelicBatchFixture::UnregisteredRelic : public ClosedTypedRelicAutomation<UnregisteredRelic>
{
protected:
    void InitializeImplementation() override {}
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
        static const TypeName typeName;
    };

    template<>
    struct Traits<::RelicBatchFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::RelicBatchFixture::UnregisteredRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::RelicBatchFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeName typeName;
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