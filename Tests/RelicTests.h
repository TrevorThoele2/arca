#pragma once

#include "GeneralFixture.h"

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
    class DefaultConstructedRelic;
    class RelicConstructedFromMovedValue;
    class RelicWithShouldCreateAndMovedValue;
};

namespace Arca
{
    template<>
    struct Traits<RelicTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "RelicTestsFixture::Shard";
    };

    template<>
    struct Traits<RelicTestsFixture::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "RelicTestsFixture::OtherShard";
    };

    template<>
    struct Traits<RelicTestsFixture::TypedClosedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicTestsFixture::TypedClosedRelic";
    };

    template<>
    struct Traits<RelicTestsFixture::TypedOpenRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicTestsFixture::TypedOpenRelic";
        static const Openness openness = Openness::Open;
    };

    template<>
    struct Traits<RelicTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicTestsFixture::GlobalRelic";
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<RelicTestsFixture::ShouldCreateRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicTestsFixture::ShouldCreateRelic";
        static bool ShouldCreate(Reliquary& reliquary, int value);
    };

    template<>
    struct Traits<RelicTestsFixture::InitializedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicTestsFixture::InitializedRelic";
    };

    template<>
    struct Traits<RelicTestsFixture::MovableOnlyRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicTestsFixture::MovableOnlyRelic";
    };

    template<>
    struct Traits<RelicTestsFixture::DefaultConstructedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicTestsFixture::DefaultConstructedRelic";
    };

    template<>
    struct Traits<RelicTestsFixture::RelicConstructedFromMovedValue>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicTestsFixture::RelicConstructedFromMoveValue";
    };

    template<>
    struct Traits<RelicTestsFixture::RelicWithShouldCreateAndMovedValue>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicTestsFixture::RelicWithShouldCreateAndMovedValue";
        static bool ShouldCreate(Reliquary& reliquary, std::unique_ptr<int>& myInt);
    };
}

class RelicTestsFixture::TypedClosedRelic final
{
public:
    Index<Shard> basicShard;
public:
    explicit TypedClosedRelic(RelicInit init);
};

class RelicTestsFixture::TypedOpenRelic final
{
public:
    Index<Shard> basicShard;
public:
    explicit TypedOpenRelic(RelicInit init);
};

class RelicTestsFixture::GlobalRelic final
{
public:
    Index<Shard> basicShard;
public:
    explicit GlobalRelic(RelicInit init);
};

class RelicTestsFixture::ShouldCreateRelic final
{
public:
    int value = 0;
public:
    explicit ShouldCreateRelic(RelicInit init);
    ShouldCreateRelic(RelicInit init, int value);
};

class RelicTestsFixture::InitializedRelic final
{
public:
    Index<Shard> basicShard;

    int myValue = 0;
public:
    explicit InitializedRelic(RelicInit init);
    InitializedRelic(RelicInit init, int value);
};

class RelicTestsFixture::MovableOnlyRelic final
{
public:
    Index<Shard> basicShard;

    int myValue = 0;
public:
    explicit MovableOnlyRelic(RelicInit init);
    MovableOnlyRelic(RelicInit init, int myInt);
    MovableOnlyRelic(const MovableOnlyRelic& arg) = delete;
    MovableOnlyRelic(MovableOnlyRelic&& arg) noexcept = default;
    MovableOnlyRelic& operator=(const MovableOnlyRelic& arg) = delete;
    MovableOnlyRelic& operator=(MovableOnlyRelic&& arg) noexcept = default;
};

class RelicTestsFixture::DefaultConstructedRelic final
{
public:
    DefaultConstructedRelic() = default;
};

class RelicTestsFixture::RelicConstructedFromMovedValue final
{
public:
    std::unique_ptr<int> myInt;
public:
    explicit RelicConstructedFromMovedValue(RelicInit init);
    RelicConstructedFromMovedValue(RelicInit init, std::unique_ptr<int>&& myInt);
};

class RelicTestsFixture::RelicWithShouldCreateAndMovedValue final
{
public:
    std::unique_ptr<int> myInt;
public:
    explicit RelicWithShouldCreateAndMovedValue(RelicInit init);
    RelicWithShouldCreateAndMovedValue(RelicInit init, std::unique_ptr<int>&& myInt);
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
    struct ScribeTraits<RelicTestsFixture::DefaultConstructedRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<RelicTestsFixture::DefaultConstructedRelic>;
    };

    template<class Archive>
    struct ScribeTraits<RelicTestsFixture::RelicConstructedFromMovedValue, Archive> final
    {
        using Category = ArcaNullScribeCategory<RelicTestsFixture::RelicConstructedFromMovedValue>;
    };

    template<class Archive>
    struct ScribeTraits<RelicTestsFixture::RelicWithShouldCreateAndMovedValue, Archive> final
    {
        using Category = ArcaNullScribeCategory<RelicTestsFixture::RelicWithShouldCreateAndMovedValue>;
    };
}