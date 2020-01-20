#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelicAutomation.h>
#include <Arca/OpenTypedRelicAutomation.h>

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

    class TypedRelic final : public ClosedTypedRelicAutomation<TypedRelic>
    {
    public:
        ShardIndex<Shard> basicShard;
    public:
        TypedRelic() = default;

        void PostConstruct();
        void Initialize();
    };

    class OpenTypedRelic final : public OpenTypedRelicAutomation<OpenTypedRelic>
    {
    public:
        ShardIndex<Shard> basicShard;
    public:
        OpenTypedRelic() = default;

        void PostConstruct();
        void Initialize();
    };

    class GlobalRelic final : public ClosedTypedRelicAutomation<GlobalRelic>
    {
    public:
        ShardIndex<Shard> basicShard;
    public:
        GlobalRelic() = default;

        void PostConstruct();
        void Initialize();
    };

    class ShouldCreateRelic final : public ClosedTypedRelicAutomation<ShouldCreateRelic>
    {
    public:
        int value = 0;
    public:
        ShouldCreateRelic() = default;

        void Initialize(int value);
    };

    class InitializedRelic final : public ClosedTypedRelicAutomation<InitializedRelic>
    {
    public:
        ShardIndex<Shard> basicShard;

        int myValue = 0;
    public:
        void PostConstruct();
        void Initialize(int myValue);
    };

    class MovableOnlyRelic final : public ClosedTypedRelicAutomation<MovableOnlyRelic>
    {
    public:
        ShardIndex<Shard> basicShard;

        int myValue = 0;
    public:
        MovableOnlyRelic() = default;
        MovableOnlyRelic(const MovableOnlyRelic& arg) = delete;
        MovableOnlyRelic(MovableOnlyRelic&& arg) noexcept = default;
        MovableOnlyRelic& operator=(const MovableOnlyRelic& arg) = delete;
        MovableOnlyRelic& operator=(MovableOnlyRelic&& arg) noexcept = default;

        void PostConstruct();
        void Initialize();
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
    struct Traits<::RelicTestsFixture::TypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicTestsTypedRelic";
    };

    template<>
    struct Traits<::RelicTestsFixture::OpenTypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicTestsOpenTypedRelic";
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

    template<>
    class Scribe<::RelicTestsFixture::MovableOnlyRelic, BinaryArchive> final
        : public ArcaNullScribe<::RelicTestsFixture::MovableOnlyRelic, BinaryArchive>
    {};
}