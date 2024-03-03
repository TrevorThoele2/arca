#pragma once

#include "ReliquaryFixture.h"

#include <Arca/Shard.h>
#include <Arca/TypedRelicAutomation.h>

using namespace Arca;

class CompositeShardBatchFixture : public ReliquaryFixture
{
public:
    template<size_t i>
    class Shard;
    class Relic;
    class GlobalRelic;
};

template<size_t i>
class CompositeShardBatchFixture::Shard
{
public:
    int value = 0;
public:
    Shard() = default;
    explicit Shard(int value);
};

template<size_t i>
CompositeShardBatchFixture::Shard<i>::Shard(int value) :
    value(value)
{}

class CompositeShardBatchFixture::Relic : public TypedRelicAutomation<Relic, Shard<0>, Shard<1>, Shard<2>>
{};

class CompositeShardBatchFixture::GlobalRelic : public TypedRelicAutomation<Relic, Shard<0>, Shard<1>, Shard<2>>
{};

namespace Arca
{
    template<size_t i>
    struct Traits<::CompositeShardBatchFixture::Shard<i>>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeHandleName typeName;
    };

    template<size_t i>
    const TypeHandleName Traits<CompositeShardBatchFixture::Shard<i>>::typeName
        = "EitherShardBatchTestsShard" + ::Chroma::ToString(i);

    template<>
    struct Traits<::CompositeShardBatchFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::CompositeShardBatchFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
        static const bool isGlobal = true;
    };
}

namespace Inscription
{
    template<size_t i>
    class Scribe<::CompositeShardBatchFixture::Shard<i>, BinaryArchive> final
        : public ShardScribe<::CompositeShardBatchFixture::Shard<i>, BinaryArchive>
    {
    private:
        using BaseT = ShardScribe<::CompositeShardBatchFixture::Shard<i>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;

        using BaseT::Scriven;
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.value);
        }
    };

    template<>
    class Scribe<::CompositeShardBatchFixture::Relic, BinaryArchive> final
        : public CompositeRelicScribe<::CompositeShardBatchFixture::Relic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };

    template<>
    class Scribe<::CompositeShardBatchFixture::GlobalRelic, BinaryArchive> final
        : public CompositeRelicScribe<::CompositeShardBatchFixture::GlobalRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };
}