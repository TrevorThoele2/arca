#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelicAutomation.h>

#include <Arca/Serialization.h>

using namespace Arca;

class CompositeShardTestsFixture : public GeneralFixture
{
public:
    template<size_t i>
    class Shard
    {
    public:
        std::string myValue;
    public:
        Shard() = default;
        explicit Shard(std::string myValue);
    };

    class BasicTypedRelic final : public ClosedTypedRelicAutomation<BasicTypedRelic, Shard<0>, Shard<1>, Shard<2>>
    {
    public:
        LocalPtr<Shard<0>> shard0;
        LocalPtr<Shard<1>> shard1;
        LocalPtr<Shard<2>> shard2;
    public:
        BasicTypedRelic() = default;

        void PostConstruct(ShardTuple shards);
    };
};

template<size_t i>
CompositeShardTestsFixture::Shard<i>::Shard(std::string myValue) : myValue(std::move(myValue))
{}

namespace Arca
{
    template<size_t i>
    struct Traits<::CompositeShardTestsFixture::Shard<i>>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "RelicTestsBasicShard" + ::Chroma::ToString(i);
    };

    template<>
    struct Traits<::CompositeShardTestsFixture::BasicTypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsBasicTypedRelic";
    };
}

namespace Inscription
{
    template<size_t i>
    class Scribe<::CompositeShardTestsFixture::Shard<i>, BinaryArchive> final
        : public ArcaCompositeScribe<::CompositeShardTestsFixture::Shard<i>, BinaryArchive>
    {
    private:
        using BaseT = ArcaCompositeScribe<::CompositeShardTestsFixture::Shard<i>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;

        using BaseT::Scriven;
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::CompositeShardTestsFixture::BasicTypedRelic, BinaryArchive> final
        : public ArcaNullScribe<::CompositeShardTestsFixture::BasicTypedRelic, BinaryArchive>
    {};
}