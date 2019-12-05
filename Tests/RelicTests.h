#pragma once

#include "GeneralFixture.h"

#include <Arca/TypedRelicAutomation.h>
#include <Arca/Shard.h>

#include <Arca/Serialization.h>

using namespace Arca;

class RelicTestsFixture : public GeneralFixture
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

    class OtherShard
    {
    public:
        int myValue;
    public:
        OtherShard() = default;
        explicit OtherShard(int myValue);
    };

    class BasicTypedRelic : public TypedRelicAutomation<BasicTypedRelic, BasicShard>
    {
    public:
        LocalPtr<BasicShard> basicShard;
    public:
        BasicTypedRelic() = default;
    protected:
        void InitializeImplementation() override;
    };

    class GlobalRelic : public TypedRelicAutomation<GlobalRelic, BasicShard>
    {
    public:
        LocalPtr<BasicShard> basicShard;
    public:
        GlobalRelic() = default;
    protected:
        void InitializeImplementation() override;
    };

    class MostBasicCustomFactoryRelic : public TypedRelicAutomation<MostBasicCustomFactoryRelic>
    {
    public:
        int value = 0;
    public:
        MostBasicCustomFactoryRelic() = default;
    protected:
        void InitializeImplementation() override {}
    };

    class GuardedCustomFactoryRelic : public TypedRelicAutomation<GuardedCustomFactoryRelic>
    {
    public:
        int value = 0;
    public:
        GuardedCustomFactoryRelic() = default;
    protected:
        void InitializeImplementation() override {}
    };
};

namespace Arca
{
    template<>
    struct Traits<::RelicTestsFixture::BasicShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::RelicTestsFixture::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::RelicTestsFixture::BasicTypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::RelicTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::RelicTestsFixture::MostBasicCustomFactoryRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
        static std::optional<RelicTestsFixture::MostBasicCustomFactoryRelic>
            Factory(Reliquary& reliquary);
    };

    template<>
    struct Traits<::RelicTestsFixture::GuardedCustomFactoryRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
        static std::optional<RelicTestsFixture::GuardedCustomFactoryRelic>
            Factory(Reliquary& reliquary, int value);
    };
}

namespace Inscription
{
    template<>
    class Scribe<::RelicTestsFixture::BasicShard, BinaryArchive> final
        : public ShardScribe<::RelicTestsFixture::BasicShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::RelicTestsFixture::OtherShard, BinaryArchive> final
        : public ShardScribe<::RelicTestsFixture::OtherShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::RelicTestsFixture::BasicTypedRelic, BinaryArchive> final
        : public CompositeRelicScribe<::RelicTestsFixture::BasicTypedRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };

    template<>
    class Scribe<::RelicTestsFixture::GlobalRelic, BinaryArchive> final
        : public CompositeRelicScribe<::RelicTestsFixture::GlobalRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };

    template<>
    class Scribe<::RelicTestsFixture::MostBasicCustomFactoryRelic, BinaryArchive> final
        : public CompositeRelicScribe<::RelicTestsFixture::MostBasicCustomFactoryRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };

    template<>
    class Scribe<::RelicTestsFixture::GuardedCustomFactoryRelic, BinaryArchive> final
        : public CompositeRelicScribe<::RelicTestsFixture::GuardedCustomFactoryRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };
}