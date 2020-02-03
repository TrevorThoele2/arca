#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelic.h>

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

    class TypedRelic final : public ClosedTypedRelic<TypedRelic>
    {
    public:
        ShardIndex<Shard> basicShard;
    public:
        explicit TypedRelic(Init init);
    };

    class GlobalRelic final : public ClosedTypedRelic<GlobalRelic>
    {
    public:
        ShardIndex<Shard> basicShard;
    public:
        explicit GlobalRelic(Init init);
    };

    class Curator final : public Arca::Curator
    {
    public:
        using Arca::Curator::Curator;
    };

    struct Signal
    {};
};

namespace Arca
{
    template<>
    struct Traits<::HandleTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "HandleTestsShard";
    };

    template<>
    struct Traits<::HandleTestsFixture::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "HandleTestsOtherShard";
    };

    template<>
    struct Traits<::HandleTestsFixture::TypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "HandleTestsTypedRelic";
    };

    template<>
    struct Traits<::HandleTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "HandleTestsGlobalRelic";
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::HandleTestsFixture::Curator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "HandleTestsBasicCurator";
    };

    template<>
    struct Traits<::HandleTestsFixture::Signal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "HandleTestsBasicSignal";
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
    class Scribe<::HandleTestsFixture::Curator, BinaryArchive> final
        : public ArcaNullScribe<::HandleTestsFixture::Curator, BinaryArchive>
    {};
}