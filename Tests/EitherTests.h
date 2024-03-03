#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelicAutomation.h>
#include <Arca/ShardTraits.h>

#include <Arca/Serialization.h>

using namespace Arca;

class EitherTestsFixture : public GeneralFixture
{
public:
    class BasicShard
    {
    public:
        std::string myValue;
    public:
        BasicShard() = default;
        explicit BasicShard(std::string myValue);
    };

    class BasicTypedRelic : public ClosedTypedRelicAutomation<BasicTypedRelic, BasicShard>
    {
    public:
        Ptr<BasicShard> basicShard;
    public:
        BasicTypedRelic() = default;

        void PostConstruct(ShardTuple shards);
    };
};

namespace Arca
{
    template<>
    struct Traits<::EitherTestsFixture::BasicShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::EitherTestsFixture::BasicTypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::EitherTestsFixture::BasicShard, BinaryArchive> final
        : public ArcaCompositeScribe<::EitherTestsFixture::BasicShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::EitherTestsFixture::BasicTypedRelic, BinaryArchive> final
        : public ArcaNullScribe<::EitherTestsFixture::BasicTypedRelic, BinaryArchive>
    {};
}