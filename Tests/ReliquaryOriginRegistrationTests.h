#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelic.h>

#include <Inscription/BinaryArchive.h>

using namespace Arca;

class ReliquaryOriginRegistrationTestsFixture : public GeneralFixture
{
public:
    class Shard
    {};

    class Relic final : public ClosedTypedRelic<Relic>
    {
    public:
        explicit Relic(Init init) : ClosedTypedRelic(init)
        {}
    };

    class GlobalRelic final : public ClosedTypedRelic<GlobalRelic>
    {
    public:
        explicit GlobalRelic(Init init) : ClosedTypedRelic(init)
        {}
    };

    class Curator final : public Arca::Curator
    {
    public:
        using Arca::Curator::Curator;
    };

    struct Signal
    {};
};

namespace Arca
{
    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ReliquaryTestsShard";
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsRelic";
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsGlobalRelic";
        static const bool isGlobal = true;
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::Curator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "ReliquaryTestsCurator";
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::Signal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "ReliquaryTestsSignal";
    };
}

namespace Inscription
{
    template<>
    class Scribe<::ReliquaryOriginRegistrationTestsFixture::Shard, BinaryArchive> final
        : public ArcaNullScribe<::ReliquaryOriginRegistrationTestsFixture::Shard, BinaryArchive>
    {};

    template<>
    class Scribe<::ReliquaryOriginRegistrationTestsFixture::Relic, BinaryArchive> final
        : public ArcaNullScribe<::ReliquaryOriginRegistrationTestsFixture::Relic, BinaryArchive>
    {};

    template<>
    class Scribe<::ReliquaryOriginRegistrationTestsFixture::GlobalRelic, BinaryArchive> final
        : public ArcaNullScribe<::ReliquaryOriginRegistrationTestsFixture::GlobalRelic, BinaryArchive>
    {};

    template<>
    class Scribe<::ReliquaryOriginRegistrationTestsFixture::Curator, BinaryArchive> final
        : public ArcaNullScribe<::ReliquaryOriginRegistrationTestsFixture::Curator, BinaryArchive>
    {};
}