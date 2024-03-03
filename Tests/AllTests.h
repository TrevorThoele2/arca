#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelicAutomation.h>

#include <Arca/Serialization.h>

using namespace Arca;

class AllTestsFixture : public GeneralFixture
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

    class BasicTypedRelic final : public ClosedTypedRelicAutomation<BasicTypedRelic>
    {
    public:
        ShardIndex<Shard<0>> shard0;
        ShardIndex<Shard<1>> shard1;
        ShardIndex<Shard<2>> shard2;
    public:
        BasicTypedRelic() = default;

        void Initialize();
    };
};

template<size_t i>
AllTestsFixture::Shard<i>::Shard(std::string myValue) : myValue(std::move(myValue))
{}

namespace Arca
{
    template<size_t i>
    struct Traits<::AllTestsFixture::Shard<i>>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "RelicTestsBasicShard" + ::Chroma::ToString(i);
    };

    template<>
    struct Traits<::AllTestsFixture::BasicTypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsBasicTypedRelic";
    };
}

namespace Inscription
{
    template<size_t i>
    class Scribe<::AllTestsFixture::Shard<i>, BinaryArchive> final
        : public ArcaCompositeScribe<::AllTestsFixture::Shard<i>, BinaryArchive>
    {
    private:
        using BaseT = ArcaCompositeScribe<::AllTestsFixture::Shard<i>, BinaryArchive>;
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
    class Scribe<::AllTestsFixture::BasicTypedRelic, BinaryArchive> final
        : public ArcaNullScribe<::AllTestsFixture::BasicTypedRelic, BinaryArchive>
    {};
}