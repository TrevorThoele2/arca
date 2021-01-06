#pragma once

#include "GeneralFixture.h"

#include <Inscription/BinaryArchive.h>

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
        static const ObjectType objectType = ObjectType::Shard;
        static TypeName TypeName() { return "ReliquaryTestsBasicShard"; }
    };

    template<>
    struct Traits<ReliquaryTestsFixture::OtherBasicShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static TypeName TypeName() { return "ReliquaryTestsOtherBasicShard"; }
    };

    template<>
    struct Traits<ReliquaryTestsFixture::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static TypeName TypeName() { return "ReliquaryTestsOtherShard"; }
    };

    template<>
    struct Traits<ReliquaryTestsFixture::BasicTypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "ReliquaryTestsBasicTypedRelic"; }
    };

    template<>
    struct Traits<ReliquaryTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "ReliquaryTestsGlobalRelic"; }
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<ReliquaryTestsFixture::BasicCurator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static TypeName TypeName() { return "ReliquaryTestsBasicCurator"; }
    };

    template<>
    struct Traits<ReliquaryTestsFixture::BasicSignal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static TypeName TypeName() { return "ReliquaryTestsBasicSignal"; }
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
    template<class Archive>
    struct ScribeTraits<ReliquaryTestsFixture::BasicTypedRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<ReliquaryTestsFixture::BasicTypedRelic>;
    };

    template<class Archive>
    struct ScribeTraits<ReliquaryTestsFixture::GlobalRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<ReliquaryTestsFixture::GlobalRelic>;
    };

    template<class Archive>
    struct ScribeTraits<ReliquaryTestsFixture::BasicCurator, Archive> final
    {
        using Category = ArcaNullScribeCategory<ReliquaryTestsFixture::BasicCurator>;
    };
}