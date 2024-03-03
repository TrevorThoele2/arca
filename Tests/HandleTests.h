#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelicAutomation.h>

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

    class TypedRelic final : public ClosedTypedRelicAutomation<TypedRelic, Shard>
    {
    public:
        LocalPtr<Shard> basicShard;
    public:
        TypedRelic() = default;

        void PostConstruct(ShardTuple shards);
    };

    class GlobalRelic final : public ClosedTypedRelicAutomation<GlobalRelic, Shard>
    {
    public:
        LocalPtr<Shard> basicShard;
    public:
        GlobalRelic() = default;

        void PostConstruct(ShardTuple shards);
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
        static const TypeName typeName;
    };

    template<>
    struct Traits<::HandleTestsFixture::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::HandleTestsFixture::TypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::HandleTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::HandleTestsFixture::Curator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::HandleTestsFixture::Signal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static const TypeName typeName;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::HandleTestsFixture::Shard, BinaryArchive> final
        : public ArcaCompositeScribe<::HandleTestsFixture::Shard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::HandleTestsFixture::OtherShard, BinaryArchive> final
        : public ArcaCompositeScribe<::HandleTestsFixture::OtherShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::HandleTestsFixture::TypedRelic, BinaryArchive> final
        : public ArcaNullScribe<::HandleTestsFixture::TypedRelic, BinaryArchive>
    {};

    template<>
    class Scribe<::HandleTestsFixture::GlobalRelic, BinaryArchive> final
        : public ArcaNullScribe<::HandleTestsFixture::GlobalRelic, BinaryArchive>
    {};

    template<>
    class Scribe<::HandleTestsFixture::Curator, BinaryArchive> final :
        public ArcaNullScribe<::HandleTestsFixture::Curator, BinaryArchive>
    {};
}