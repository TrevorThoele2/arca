#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelicAutomation.h>

#include <Arca/Serialization.h>

using namespace Arca;

class GlobalRelicTestsFixture : public GeneralFixture
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

    class GlobalRelic final : public ClosedTypedRelicAutomation<GlobalRelic>
    {
    public:
        int myValue = 0;
        ShardIndex<BasicShard> basicShard;
    public:
        GlobalRelic() = default;

        void PostConstruct();
        void Initialize();
    };
};

namespace Arca
{
    template<>
    struct Traits<::GlobalRelicTestsFixture::BasicShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "GlobalRelicTestsBasicShard";
    };

    template<>
    struct Traits<::GlobalRelicTestsFixture::BasicTypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "GlobalRelicTestsBasicTypedRelic";
    };

    template<>
    struct Traits<::GlobalRelicTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "GlobalRelicTestsGlobalRelic";
        static const Locality locality = Locality::Global;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::GlobalRelicTestsFixture::BasicShard, BinaryArchive> final
        : public ArcaCompositeScribe<::GlobalRelicTestsFixture::BasicShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::GlobalRelicTestsFixture::BasicTypedRelic, BinaryArchive> final
        : public ArcaNullScribe<::GlobalRelicTestsFixture::BasicTypedRelic, BinaryArchive>
    {};

    template<>
    class Scribe<::GlobalRelicTestsFixture::GlobalRelic, BinaryArchive> final
        : public ArcaNullScribe<::GlobalRelicTestsFixture::GlobalRelic, BinaryArchive>
    {};
}