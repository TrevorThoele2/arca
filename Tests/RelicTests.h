#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelic.h>
#include <Arca/OpenTypedRelic.h>

#include <Arca/Serialization.h>

#include "DifferentiableShard.h"

using namespace Arca;

class RelicTestsFixture : public GeneralFixture
{
public:
    using Shard = DifferentiableShard<0>;
    using OtherShard = DifferentiableShard<1>;

    class TypedClosedRelic;
    class TypedOpenRelic;
    class GlobalRelic;
    class ShouldCreateRelic;
    class InitializedRelic;
    class MovableOnlyRelic;
    class RelicConstructedFromMovedValue;
    class ClosedTypedRelicWithUsingConstructor;
    class OpenTypedRelicWithUsingConstructor;
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

    template<>
    struct Traits<::RelicTestsFixture::RelicConstructedFromMovedValue>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsRelicConstructedFromMoveValue";
    };

    template<>
    struct Traits<::RelicTestsFixture::ClosedTypedRelicWithUsingConstructor>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsClosedTypedRelicWithUsingConstructor";
    };

    template<>
    struct Traits<::RelicTestsFixture::OpenTypedRelicWithUsingConstructor>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsOpenTypedRelicWithUsingConstructor";
    };
}

class RelicTestsFixture::TypedClosedRelic final : public ClosedTypedRelic<TypedClosedRelic>
{
public:
    Index<Shard> basicShard;
public:
    explicit TypedClosedRelic(Init init);
};

class RelicTestsFixture::TypedOpenRelic final : public OpenTypedRelic<TypedOpenRelic>
{
public:
    Index<Shard> basicShard;
public:
    explicit TypedOpenRelic(Init init);
};

class RelicTestsFixture::GlobalRelic final : public ClosedTypedRelic<GlobalRelic>
{
public:
    Index<Shard> basicShard;
public:
    explicit GlobalRelic(Init init);
};

class RelicTestsFixture::ShouldCreateRelic final : public ClosedTypedRelic<ShouldCreateRelic>
{
public:
    int value = 0;
public:
    explicit ShouldCreateRelic(Init init) : ClosedTypedRelic(init)
    {}
    ShouldCreateRelic(Init init, int value);
};

class RelicTestsFixture::InitializedRelic final : public ClosedTypedRelic<InitializedRelic>
{
public:
    Index<Shard> basicShard;

    int myValue = 0;
public:
    explicit InitializedRelic(Init init);
    InitializedRelic(Init init, int value);
};

class RelicTestsFixture::MovableOnlyRelic final : public ClosedTypedRelic<MovableOnlyRelic>
{
public:
    Index<Shard> basicShard;

    int myValue = 0;
public:
    explicit MovableOnlyRelic(Init init);
    MovableOnlyRelic(Init init, int myInt);
    MovableOnlyRelic(const MovableOnlyRelic& arg) = delete;
    MovableOnlyRelic(MovableOnlyRelic&& arg) noexcept = default;
    MovableOnlyRelic& operator=(const MovableOnlyRelic& arg) = delete;
    MovableOnlyRelic& operator=(MovableOnlyRelic&& arg) noexcept = default;
};

class RelicTestsFixture::RelicConstructedFromMovedValue final : public ClosedTypedRelic<RelicConstructedFromMovedValue>
{
public:
    std::unique_ptr<int> myInt;
public:
    explicit RelicConstructedFromMovedValue(Init init) : ClosedTypedRelic(init)
    {}

    RelicConstructedFromMovedValue(Init init, std::unique_ptr<int>&& myInt);
};

class RelicTestsFixture::ClosedTypedRelicWithUsingConstructor final : public ClosedTypedRelic<ClosedTypedRelicWithUsingConstructor>
{
public:
    using ClosedTypedRelic::ClosedTypedRelic;
};

class RelicTestsFixture::OpenTypedRelicWithUsingConstructor final : public OpenTypedRelic<OpenTypedRelicWithUsingConstructor>
{
public:
    using OpenTypedRelic::OpenTypedRelic;
};

namespace Inscription
{
    template<class Archive>
    struct ScribeTraits<RelicTestsFixture::TypedClosedRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<RelicTestsFixture::TypedClosedRelic>;
    };

    template<class Archive>
    struct ScribeTraits<RelicTestsFixture::TypedOpenRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<RelicTestsFixture::TypedOpenRelic>;
    };

    template<class Archive>
    struct ScribeTraits<RelicTestsFixture::GlobalRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<RelicTestsFixture::GlobalRelic>;
    };

    template<class Archive>
    struct ScribeTraits<RelicTestsFixture::ShouldCreateRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<RelicTestsFixture::ShouldCreateRelic>;
    };

    template<class Archive>
    struct ScribeTraits<RelicTestsFixture::InitializedRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<RelicTestsFixture::InitializedRelic>;
    };

    template<class Archive>
    struct ScribeTraits<RelicTestsFixture::MovableOnlyRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<RelicTestsFixture::MovableOnlyRelic>;
    };

    template<class Archive>
    struct ScribeTraits<RelicTestsFixture::RelicConstructedFromMovedValue, Archive> final
    {
        using Category = ArcaNullScribeCategory<RelicTestsFixture::RelicConstructedFromMovedValue>;
    };

    template<class Archive>
    struct ScribeTraits<RelicTestsFixture::ClosedTypedRelicWithUsingConstructor, Archive> final
    {
        using Category = ArcaNullScribeCategory<RelicTestsFixture::ClosedTypedRelicWithUsingConstructor>;
    };

    template<class Archive>
    struct ScribeTraits<RelicTestsFixture::OpenTypedRelicWithUsingConstructor, Archive> final
    {
        using Category = ArcaNullScribeCategory<RelicTestsFixture::OpenTypedRelicWithUsingConstructor>;
    };
}