#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelicAutomation.h>
#include <Arca/Shard.h>

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

    class BasicTypedRelic : public ClosedTypedRelicAutomation<BasicTypedRelic, BasicShard>
    {
    public:
        Ptr<BasicShard> basicShard;
    public:
        BasicTypedRelic() = default;
    protected:
        void InitializeImplementation() override;
    };

    class GlobalRelic : public ClosedTypedRelicAutomation<GlobalRelic, BasicShard>
    {
    public:
        Ptr<BasicShard> basicShard;
    public:
        GlobalRelic() = default;
    protected:
        void InitializeImplementation() override;
    };
};

namespace Arca
{
    template<>
    struct Traits<::GlobalRelicTestsFixture::BasicShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::GlobalRelicTestsFixture::BasicTypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::GlobalRelicTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
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