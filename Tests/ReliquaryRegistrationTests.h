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
        static const inline TypeName typeName = "ReliquaryTestsShard";
    };

    template<>
    struct Traits<ReliquaryRegistrationTestsFixture::Relic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "ReliquaryTestsRelic";
    };

    template<>
    struct Traits<ReliquaryRegistrationTestsFixture::GlobalRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "ReliquaryTestsGlobalRelic";
        static constexpr Locality locality = Locality::Global;
    };

    template<>
    struct Traits<ReliquaryRegistrationTestsFixture::Curator>
    {
        static constexpr ObjectType objectType = ObjectType::Curator;
        static const inline TypeName typeName = "ReliquaryTestsCurator";
    };

    template<>
    struct Traits<ReliquaryRegistrationTestsFixture::Command>
    {
        static constexpr ObjectType objectType = ObjectType::Command;
        static const inline TypeName typeName = "ReliquaryTestsCommand";
    };

    template<>
    struct Traits<ReliquaryRegistrationTestsFixture::Signal>
    {
        static constexpr ObjectType objectType = ObjectType::Signal;
        static const inline TypeName typeName = "ReliquaryTestsSignal";
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