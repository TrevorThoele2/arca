#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelicAutomation.h>
#include <Arca/ShardTraits.h>

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

    class BasicTypedRelic : public ClosedTypedRelicAutomation<BasicTypedRelic, Shard<0>, Shard<1>, Shard<2>>
    {
    public:
        Shard<0>* shard0;
        Shard<1>* shard1;
        Shard<2>* shard2;
    public:
        BasicTypedRelic() = default;
    protected:
        void InitializeImplementation() override;
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
        static const TypeName typeName;
    };

    template<size_t i>
    const TypeName Traits<CompositeShardTestsFixture::Shard<i>>::typeName =
        "RelicTestsBasicShard" + ::Chroma::ToString(i);

    template<>
    struct Traits<::CompositeShardTestsFixture::BasicTypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
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