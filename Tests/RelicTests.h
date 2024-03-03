#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelicAutomation.h>
#include <Arca/OpenTypedRelicAutomation.h>
#include <Arca/ShardTraits.h>

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
        Shard* basicShard;
    public:
        TypedRelic() = default;

        void PostConstruct(ShardTuple shards);
    };

    class OpenTypedRelic : public OpenTypedRelicAutomation<OpenTypedRelic, Shard>
    {
    public:
        Shard* basicShard;
    public:
        OpenTypedRelic() = default;

        void PostConstruct(ShardTuple shards);
    };

    class GlobalRelic : public ClosedTypedRelicAutomation<GlobalRelic, Shard>
    {
    public:
        Shard* basicShard;
    public:
        GlobalRelic() = default;

        void PostConstruct(ShardTuple shards);
    };

    class ShouldCreateRelic : public ClosedTypedRelicAutomation<ShouldCreateRelic>
    {
    public:
        int value = 0;
    public:
        ShouldCreateRelic() = default;

        void Initialize(int value);
    };

    class InitializedRelic : public ClosedTypedRelicAutomation<InitializedRelic, Shard>
    {
    public:
        Shard* basicShard;

        int myValue = 0;
    public:
        void PostConstruct(ShardTuple shards);

        void Initialize(int myValue);
    };
};

namespace Arca
{
    template<>
    struct Traits<::RelicTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::RelicTestsFixture::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::RelicTestsFixture::TypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::RelicTestsFixture::OpenTypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::RelicTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::RelicTestsFixture::ShouldCreateRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
        static bool ShouldCreate(Reliquary& reliquary, int value);
    };

    template<>
    struct Traits<::RelicTestsFixture::InitializedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::RelicTestsFixture::Shard, BinaryArchive> final
        : public ArcaCompositeScribe<::RelicTestsFixture::Shard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::RelicTestsFixture::OtherShard, BinaryArchive> final
        : public ArcaCompositeScribe<::RelicTestsFixture::OtherShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::RelicTestsFixture::TypedRelic, BinaryArchive> final
        : public ArcaNullScribe<::RelicTestsFixture::TypedRelic, BinaryArchive>
    {};

    template<>
    class Scribe<::RelicTestsFixture::OpenTypedRelic, BinaryArchive> final
        : public ArcaNullScribe<::RelicTestsFixture::OpenTypedRelic, BinaryArchive>
    {};

    template<>
    class Scribe<::RelicTestsFixture::GlobalRelic, BinaryArchive> final
        : public ArcaNullScribe<::RelicTestsFixture::GlobalRelic, BinaryArchive>
    {};

    template<>
    class Scribe<::RelicTestsFixture::ShouldCreateRelic, BinaryArchive> final
        : public ArcaNullScribe<::RelicTestsFixture::ShouldCreateRelic, BinaryArchive>
    {};

    template<>
    class Scribe<::RelicTestsFixture::InitializedRelic, BinaryArchive> final
        : public ArcaNullScribe<::RelicTestsFixture::InitializedRelic, BinaryArchive>
    {};
}