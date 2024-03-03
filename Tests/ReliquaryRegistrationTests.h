#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelic.h>

#include <Inscription/BinaryArchive.h>

using namespace Arca;

class ReliquaryRegistrationTestsFixture : public GeneralFixture
{
public:
    class Shard
    {};

    class Relic final : public ClosedTypedRelic<Relic>
    {
    public:
        explicit Relic(Init init);
    };

    class GlobalRelic final : public ClosedTypedRelic<Relic>
    {};

    class Curator final : public Arca::Curator
    {};

    struct Command
    {};

    struct Signal
    {};
};

namespace Arca
{
    template<>
    struct Traits<::ReliquaryRegistrationTestsFixture::Shard>
    {
        static constexpr ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ReliquaryTestsShard";
    };

    template<>
    struct Traits<::ReliquaryRegistrationTestsFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsRelic";
    };

    template<>
    struct Traits<::ReliquaryRegistrationTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsGlobalRelic";
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::ReliquaryRegistrationTestsFixture::Curator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "ReliquaryTestsCurator";
    };

    template<>
    struct Traits<::ReliquaryRegistrationTestsFixture::Command>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "ReliquaryTestsCommand";
    };

    template<>
    struct Traits<::ReliquaryRegistrationTestsFixture::Signal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "ReliquaryTestsSignal";
    };
}

namespace Inscription
{
    template<>
    class Scribe<::ReliquaryRegistrationTestsFixture::Shard, BinaryArchive> final
        : public ArcaNullScribe<::ReliquaryRegistrationTestsFixture::Shard, BinaryArchive>
    {};

    template<>
    class Scribe<::ReliquaryRegistrationTestsFixture::Relic, BinaryArchive> final
        : public ArcaNullScribe<::ReliquaryRegistrationTestsFixture::Relic, BinaryArchive>
    {};

    template<>
    class Scribe<::ReliquaryRegistrationTestsFixture::GlobalRelic, BinaryArchive> final
        : public ArcaNullScribe<::ReliquaryRegistrationTestsFixture::GlobalRelic, BinaryArchive>
    {};

    template<>
    class Scribe<::ReliquaryRegistrationTestsFixture::Curator, BinaryArchive> final
        : public ArcaNullScribe<::ReliquaryRegistrationTestsFixture::Curator, BinaryArchive>
    {};
}