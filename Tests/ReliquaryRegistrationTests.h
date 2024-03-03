#pragma once

#include "GeneralFixture.h"

#include <Inscription/BinaryArchive.h>

using namespace Arca;

class ReliquaryRegistrationTestsFixture : public GeneralFixture
{
public:
    class Shard;
    class Relic;
    class GlobalRelic;
    class Curator;
    struct Command;
    struct Signal;
};

namespace Arca
{
    template<>
    struct Traits<ReliquaryRegistrationTestsFixture::Shard>
    {
        static constexpr ObjectType objectType = ObjectType::Shard;
        static TypeName TypeName() { return "ReliquaryTestsShard"; }
    };

    template<>
    struct Traits<ReliquaryRegistrationTestsFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "ReliquaryTestsRelic"; }
    };

    template<>
    struct Traits<ReliquaryRegistrationTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "ReliquaryTestsGlobalRelic"; }
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<ReliquaryRegistrationTestsFixture::Curator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static TypeName TypeName() { return "ReliquaryTestsCurator"; }
    };

    template<>
    struct Traits<ReliquaryRegistrationTestsFixture::Command>
    {
        static const ObjectType objectType = ObjectType::Command;
        static TypeName TypeName() { return "ReliquaryTestsCommand"; }
    };

    template<>
    struct Traits<ReliquaryRegistrationTestsFixture::Signal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static TypeName TypeName() { return "ReliquaryTestsSignal"; }
    };
}

class ReliquaryRegistrationTestsFixture::Shard
{};

class ReliquaryRegistrationTestsFixture::Relic final
{
public:
    explicit Relic(RelicInit init);
};

class ReliquaryRegistrationTestsFixture::GlobalRelic final
{
public:
    explicit GlobalRelic(RelicInit init);
};

class ReliquaryRegistrationTestsFixture::Curator final : public Arca::Curator
{};

struct ReliquaryRegistrationTestsFixture::Command
{};

struct ReliquaryRegistrationTestsFixture::Signal
{};

namespace Inscription
{
    template<class Archive>
    struct ScribeTraits<ReliquaryRegistrationTestsFixture::Shard, Archive> final
    {
        using Category = ArcaNullScribeCategory<ReliquaryRegistrationTestsFixture::Shard>;
    };

    template<class Archive>
    struct ScribeTraits<ReliquaryRegistrationTestsFixture::Relic, Archive> final
    {
        using Category = ArcaNullScribeCategory<ReliquaryRegistrationTestsFixture::Relic>;
    };

    template<class Archive>
    struct ScribeTraits<ReliquaryRegistrationTestsFixture::GlobalRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<ReliquaryRegistrationTestsFixture::GlobalRelic>;
    };

    template<class Archive>
    struct ScribeTraits<ReliquaryRegistrationTestsFixture::Curator, Archive> final
    {
        using Category = ArcaNullScribeCategory<ReliquaryRegistrationTestsFixture::Curator>;
    };
}