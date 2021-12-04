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

    class LocalRelic;
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
        static constexpr ObjectType objectType = ObjectType::Shard;
        static const inline TypeName typeName = "RelicTestsFixture::Shard";
    };

    template<>
    struct Traits<RelicTestsFixture::OtherShard>
    {
        static constexpr ObjectType objectType = ObjectType::Shard;
        static const inline TypeName typeName = "RelicTestsFixture::OtherShard";
    };

    template<>
    struct Traits<RelicTestsFixture::LocalRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "RelicTestsFixture::TypedClosedRelic";
    };
    
    template<>
    struct Traits<RelicTestsFixture::GlobalRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "RelicTestsFixture::GlobalRelic";
        static constexpr Locality locality = Locality::Global;
    };

    template<>
    struct Traits<RelicTestsFixture::ShouldCreateRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "RelicTestsFixture::ShouldCreateRelic";
        static bool ShouldCreate(Reliquary& reliquary, int value);
    };

    template<>
    struct Traits<RelicTestsFixture::InitializedRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "RelicTestsFixture::InitializedRelic";
    };

    template<>
    struct Traits<RelicTestsFixture::MovableOnlyRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "RelicTestsFixture::MovableOnlyRelic";
    };

    template<>
    struct Traits<RelicTestsFixture::DefaultConstructedRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "RelicTestsFixture::DefaultConstructedRelic";
    };

    template<>
    struct Traits<RelicTestsFixture::RelicConstructedFromMovedValue>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "RelicTestsFixture::RelicConstructedFromMoveValue";
    };

    template<>
    struct Traits<RelicTestsFixture::RelicWithShouldCreateAndMovedValue>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "RelicTestsFixture::RelicWithShouldCreateAndMovedValue";
        static bool ShouldCreate(Reliquary& reliquary, std::unique_ptr<int>& myInt);
    };
}

class RelicTestsFixture::LocalRelic final
{
public:
    Index<Shard> basicShard;
public:
    explicit LocalRelic(RelicInit init);
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
    struct ScribeTraits<RelicTestsFixture::LocalRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<RelicTestsFixture::LocalRelic>;
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