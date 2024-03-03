#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelicAutomation.h>
#include <Arca/OpenTypedRelicAutomation.h>
#include <Arca/Shard.h>

#include <Arca/Serialization.h>

using namespace Arca;

class RelicTestsFixture : public GeneralFixture
{
public:
    class Shard
    {
    public:
        std::string myValue;
    public:
        Shard() = default;
        explicit Shard(std::string myValue);
    };

    class OtherShard
    {
    public:
        int myValue;
    public:
        OtherShard() = default;
        explicit OtherShard(int myValue);
    };

    class TypedRelic : public ClosedTypedRelicAutomation<TypedRelic, Shard>
    {
    public:
        Ptr<Shard> basicShard;
    public:
        TypedRelic() = default;
    protected:
        void InitializeImplementation() override;
    };

    class OpenTypedRelic : public OpenTypedRelicAutomation<OpenTypedRelic, Shard>
    {
    public:
        Ptr<Shard> basicShard;
    public:
        OpenTypedRelic() = default;
    protected:
        void InitializeImplementation() override;
    };

    class GlobalRelic : public ClosedTypedRelicAutomation<GlobalRelic, Shard>
    {
    public:
        Ptr<Shard> basicShard;
    public:
        GlobalRelic() = default;
    protected:
        void InitializeImplementation() override;
    };

    class MostBasicCustomFactoryRelic : public ClosedTypedRelicAutomation<MostBasicCustomFactoryRelic>
    {
    public:
        int value = 0;
    public:
        MostBasicCustomFactoryRelic() = default;
    protected:
        void InitializeImplementation() override {}
    };

    class GuardedCustomFactoryRelic : public ClosedTypedRelicAutomation<GuardedCustomFactoryRelic>
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
    struct Traits<::RelicTestsFixture::Shard>
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
    struct Traits<::RelicTestsFixture::TypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::RelicTestsFixture::OpenTypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::RelicTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
        static const Locality locality = Locality::Global;
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
    class Scribe<::RelicTestsFixture::Shard, BinaryArchive> final
        : public ShardScribe<::RelicTestsFixture::Shard, BinaryArchive>
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
    class Scribe<::RelicTestsFixture::TypedRelic, BinaryArchive> final
        : public CompositeRelicScribe<::RelicTestsFixture::TypedRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };

    template<>
    class Scribe<::RelicTestsFixture::OpenTypedRelic, BinaryArchive> final
        : public CompositeRelicScribe<::RelicTestsFixture::OpenTypedRelic, BinaryArchive>
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