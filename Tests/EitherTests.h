#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelicAutomation.h>

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

    class BasicTypedRelic final : public ClosedTypedRelicAutomation<BasicTypedRelic>
    {
    public:
        ShardIndex<BasicShard> basicShard;
    public:
        BasicTypedRelic() = default;

        void PostConstruct();
        void Initialize();
    };
};

namespace Arca
{
    template<>
    struct Traits<::EitherTestsFixture::BasicShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "RelicTestsBasicShard";
    };

    template<>
    struct Traits<::EitherTestsFixture::BasicTypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsBasicTypedRelic";
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