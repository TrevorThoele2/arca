#pragma once

#include "ReliquaryFixture.h"

#include <Arca/ShardTraits.h>
#include <Arca/ClosedTypedRelicAutomation.h>

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

class CompositeShardBatchFixture::Relic final :
    public ClosedTypedRelicAutomation<Relic, Shard<0>, Shard<1>, Shard<2>>
{};

class CompositeShardBatchFixture::GlobalRelic final :
    public ClosedTypedRelicAutomation<GlobalRelic, Shard<0>, Shard<1>, Shard<2>>
{};

namespace Arca
{
    template<size_t i>
    struct Traits<::CompositeShardBatchFixture::Shard<i>>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeName typeName;
    };

    template<size_t i>
    const TypeName Traits<CompositeShardBatchFixture::Shard<i>>::typeName
        = "EitherShardBatchTestsShard" + ::Chroma::ToString(i);

    template<>
    struct Traits<::CompositeShardBatchFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::CompositeShardBatchFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
        static const Locality locality = Locality::Global;
    };
}

namespace Inscription
{
    template<size_t i>
    class Scribe<::CompositeShardBatchFixture::Shard<i>, BinaryArchive> final
        : public ArcaCompositeScribe<::CompositeShardBatchFixture::Shard<i>, BinaryArchive>
    {
    private:
        using BaseT = ArcaCompositeScribe<::CompositeShardBatchFixture::Shard<i>, BinaryArchive>;
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
        : public ArcaNullScribe<::CompositeShardBatchFixture::Relic, BinaryArchive>
    {};

    template<>
    class Scribe<::CompositeShardBatchFixture::GlobalRelic, BinaryArchive> final
        : public ArcaNullScribe<::CompositeShardBatchFixture::GlobalRelic, BinaryArchive>
    {};
}