#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelic.h>

#include <Arca/Serialization.h>

using namespace Arca;

class AllTestsFixture : public GeneralFixture
{
public:
    template<size_t i>
    class Shard;

    class BasicTypedRelic;
};

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

template<size_t i>
class AllTestsFixture::Shard
{
public:
    std::string myValue;
public:
    Shard() = default;
    explicit Shard(std::string myValue);
};

template<size_t i>
AllTestsFixture::Shard<i>::Shard(std::string myValue) : myValue(std::move(myValue))
{}

class AllTestsFixture::BasicTypedRelic final : public ClosedTypedRelic<BasicTypedRelic>
{
public:
    Index<Shard<0>> shard0;
    Index<Shard<1>> shard1;
    Index<Shard<2>> shard2;
public:
    explicit BasicTypedRelic(Init init);
};

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