#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelic.h>
#include <Arca/OpenTypedRelic.h>

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

    class TypedClosedRelic final : public ClosedTypedRelic<TypedClosedRelic>
    {
    public:
        ShardIndex<Shard> basicShard;
    public:
        explicit TypedClosedRelic(Initialization initialization);
    };

    class TypedOpenRelic final : public OpenTypedRelic<TypedOpenRelic>
    {
    public:
        ShardIndex<Shard> basicShard;
    public:
        explicit TypedOpenRelic(Initialization initialization);
    };

    class GlobalRelic final : public ClosedTypedRelic<GlobalRelic>
    {
    public:
        ShardIndex<Shard> basicShard;
    public:
        explicit GlobalRelic(Initialization initialization);
    };

    class ShouldCreateRelic final : public ClosedTypedRelic<ShouldCreateRelic>
    {
    public:
        int value = 0;
    public:
        explicit ShouldCreateRelic(Initialization initialization) : ClosedTypedRelic(initialization)
        {}
        ShouldCreateRelic(Initialization initialization, int value);
    };

    class InitializedRelic final : public ClosedTypedRelic<InitializedRelic>
    {
    public:
        ShardIndex<Shard> basicShard;

        int myValue = 0;
    public:
        explicit InitializedRelic(Initialization initialization);
        InitializedRelic(Initialization initialization, int value);
    };

    class MovableOnlyRelic final : public ClosedTypedRelic<MovableOnlyRelic>
    {
    public:
        ShardIndex<Shard> basicShard;

        int myValue = 0;
    public:
        explicit MovableOnlyRelic(Initialization initialization);
        MovableOnlyRelic(Initialization initialization, int myInt);
        MovableOnlyRelic(const MovableOnlyRelic& arg) = delete;
        MovableOnlyRelic(MovableOnlyRelic&& arg) noexcept = default;
        MovableOnlyRelic& operator=(const MovableOnlyRelic& arg) = delete;
        MovableOnlyRelic& operator=(MovableOnlyRelic&& arg) noexcept = default;
    };
};

namespace Arca
{
    template<>
    struct Traits<::RelicTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "RelicTestsShard";
    };

    template<>
    struct Traits<::RelicTestsFixture::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "RelicTestsOtherShard";
    };

    template<>
    struct Traits<::RelicTestsFixture::TypedClosedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicTestsTypedClosedRelic";
    };

    template<>
    struct Traits<::RelicTestsFixture::TypedOpenRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicTestsTypedOpenRelic";
    };

    template<>
    struct Traits<::RelicTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicTestsGlobalRelic";
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::RelicTestsFixture::ShouldCreateRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsShouldCreateRelic";
        static bool ShouldCreate(Reliquary& reliquary, int value);
    };

    template<>
    struct Traits<::RelicTestsFixture::InitializedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsInitializedRelic";
    };

    template<>
    struct Traits<::RelicTestsFixture::MovableOnlyRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsMovableOnlyRelic";
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
    class Scribe<::RelicTestsFixture::TypedClosedRelic, BinaryArchive> final
        : public ArcaNullScribe<::RelicTestsFixture::TypedClosedRelic, BinaryArchive>
    {};

    template<>
    class Scribe<::RelicTestsFixture::TypedOpenRelic, BinaryArchive> final
        : public ArcaNullScribe<::RelicTestsFixture::TypedOpenRelic, BinaryArchive>
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

    template<>
    class Scribe<::RelicTestsFixture::MovableOnlyRelic, BinaryArchive> final
        : public ArcaNullScribe<::RelicTestsFixture::MovableOnlyRelic, BinaryArchive>
    {};
}