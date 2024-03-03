#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelicAutomation.h>

#include <Inscription/BinaryArchive.h>

using namespace Arca;

class ReliquaryRegistrationTestsFixture : public GeneralFixture
{
public:
    class Shard
    {};

    class Relic final : public ClosedTypedRelicAutomation<Relic, Shard>
    {};

    class GlobalRelic final : public ClosedTypedRelicAutomation<Relic>
    {};

    class GlobalRelicAlias final : public ClosedTypedRelicAutomation<Relic>
    {};

    class Curator final : public Arca::Curator
    {};

    struct Signal
    {};
};

namespace Arca
{
    template<>
    struct Traits<::ReliquaryRegistrationTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ReliquaryRegistrationTestsFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ReliquaryRegistrationTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::ReliquaryRegistrationTestsFixture::GlobalRelicAlias>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::ReliquaryRegistrationTestsFixture::Curator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ReliquaryRegistrationTestsFixture::Signal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static const TypeName typeName;
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
    class Scribe<::ReliquaryRegistrationTestsFixture::GlobalRelicAlias, BinaryArchive> final
        : public ArcaNullScribe<::ReliquaryRegistrationTestsFixture::GlobalRelicAlias, BinaryArchive>
    {};

    template<>
    class Scribe<::ReliquaryRegistrationTestsFixture::Curator, BinaryArchive> final :
        public ArcaNullScribe<::ReliquaryRegistrationTestsFixture::Curator, BinaryArchive>
    {};
}