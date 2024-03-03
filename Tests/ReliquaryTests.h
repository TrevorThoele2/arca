#pragma once

#include "GeneralFixture.h"

#include <Inscription/Binary.h>

#include "DifferentiableShard.h"

using namespace Arca;

class ReliquaryTestsFixture : public GeneralFixture
{
public:
    using BasicShard = DifferentiableShard<0>;
    using OtherBasicShard = DifferentiableShard<1>;
    using OtherShard = DifferentiableShard<2>;

    class BasicTypedRelic;
    class GlobalRelic;

    class BasicCurator;

    struct BasicSignal;
};

namespace Arca
{
    template<>
    struct Traits<ReliquaryTestsFixture::BasicShard>
    {
        static constexpr ObjectType objectType = ObjectType::Shard;
        static const inline TypeName typeName = "ReliquaryTestsBasicShard";
    };

    template<>
    struct Traits<ReliquaryTestsFixture::OtherBasicShard>
    {
        static constexpr ObjectType objectType = ObjectType::Shard;
        static const inline TypeName typeName = "ReliquaryTestsOtherBasicShard";
    };

    template<>
    struct Traits<ReliquaryTestsFixture::OtherShard>
    {
        static constexpr ObjectType objectType = ObjectType::Shard;
        static const inline TypeName typeName = "ReliquaryTestsOtherShard";
    };

    template<>
    struct Traits<ReliquaryTestsFixture::BasicTypedRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "ReliquaryTestsBasicTypedRelic";
    };

    template<>
    struct Traits<ReliquaryTestsFixture::GlobalRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "ReliquaryTestsGlobalRelic";
        static constexpr Locality locality = Locality::Global;
    };

    template<>
    struct Traits<ReliquaryTestsFixture::BasicCurator>
    {
        static constexpr ObjectType objectType = ObjectType::Curator;
        static const inline TypeName typeName = "ReliquaryTestsBasicCurator";
    };

    template<>
    struct Traits<ReliquaryTestsFixture::BasicSignal>
    {
        static constexpr ObjectType objectType = ObjectType::Signal;
        static const inline TypeName typeName = "ReliquaryTestsBasicSignal";
    };
}

class ReliquaryTestsFixture::BasicTypedRelic final
{
public:
    Index<BasicShard> basicShard;
public:
    explicit BasicTypedRelic(RelicInit init);
};

class ReliquaryTestsFixture::GlobalRelic final
{
public:
    Index<BasicShard> basicShard;
public:
    explicit GlobalRelic(RelicInit init);
};

class ReliquaryTestsFixture::BasicCurator final : public Curator
{
public:
    using Curator::Curator;
};

struct ReliquaryTestsFixture::BasicSignal
{};

namespace Inscription
{
    template<class Format>
    struct ScribeTraits<ReliquaryTestsFixture::BasicTypedRelic, Format> final
    {
        using Category = ArcaNullScribeCategory<ReliquaryTestsFixture::BasicTypedRelic>;
    };

    template<class Format>
    struct ScribeTraits<ReliquaryTestsFixture::GlobalRelic, Format> final
    {
        using Category = ArcaNullScribeCategory<ReliquaryTestsFixture::GlobalRelic>;
    };

    template<class Format>
    struct ScribeTraits<ReliquaryTestsFixture::BasicCurator, Format> final
    {
        using Category = ArcaNullScribeCategory<ReliquaryTestsFixture::BasicCurator>;
    };
}