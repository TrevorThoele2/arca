#pragma once

#include "GeneralFixture.h"

#include <Arca/TypedRelicAutomation.h>
#include <Arca/Shard.h>

#include <Inscription/BinaryArchive.h>

using namespace Arca;

class HandleTestsFixture : public GeneralFixture
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
        std::string myValue;
    public:
        OtherShard() = default;
        explicit OtherShard(std::string myValue);
    };

    class TypedRelic : public TypedRelicAutomation<TypedRelic, Shard>
    {
    public:
        Ptr<Shard> basicShard;
    public:
        TypedRelic() = default;
    protected:
        void InitializeImplementation() override;
    };

    class GlobalRelic : public TypedRelicAutomation<GlobalRelic, Shard>
    {
    public:
        Ptr<Shard> basicShard;
    public:
        GlobalRelic() = default;
    protected:
        void InitializeImplementation() override;
    };

    class Curator final : public Arca::Curator
    {};

    struct Signal
    {};
};

namespace Arca
{
    template<>
    struct Traits<::HandleTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::HandleTestsFixture::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::HandleTestsFixture::TypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::HandleTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
        static const bool isGlobal = true;
    };

    template<>
    struct Traits<::HandleTestsFixture::Curator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::HandleTestsFixture::Signal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static const TypeHandleName typeName;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::HandleTestsFixture::Shard, BinaryArchive> final
        : public ShardScribe<::HandleTestsFixture::Shard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::HandleTestsFixture::OtherShard, BinaryArchive> final
        : public ShardScribe<::HandleTestsFixture::OtherShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::HandleTestsFixture::TypedRelic, BinaryArchive> final
        : public CompositeRelicScribe<::HandleTestsFixture::TypedRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };

    template<>
    class Scribe<::HandleTestsFixture::GlobalRelic, BinaryArchive> final
        : public CompositeRelicScribe<::HandleTestsFixture::GlobalRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };

    template<>
    class Scribe<::HandleTestsFixture::Curator, BinaryArchive> final :
        public CuratorScribe<::HandleTestsFixture::Curator, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };
}