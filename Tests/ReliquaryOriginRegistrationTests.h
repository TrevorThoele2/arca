#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelicAutomation.h>
#include <Arca/Shard.h>

#include <Inscription/BinaryArchive.h>
#include "Inscription/BaseScriven.h"

using namespace Arca;

class ReliquaryOriginRegistrationTestsFixture : public GeneralFixture
{
public:
    class Shard
    {};

    class Relic final : public ClosedTypedRelicAutomation<Relic>
    {};

    class GlobalRelic final : public ClosedTypedRelicAutomation<GlobalRelic>
    {};

    class Curator final : public Arca::Curator
    {};

    struct Signal
    {};
};

namespace Arca
{
    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
        static const bool isGlobal = true;
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::Curator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::Signal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static const TypeHandleName typeName;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::ReliquaryOriginRegistrationTestsFixture::Shard, BinaryArchive> final
        : public ShardScribe<::ReliquaryOriginRegistrationTestsFixture::Shard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };

    template<>
    class Scribe<::ReliquaryOriginRegistrationTestsFixture::Relic, BinaryArchive> final
        : public CompositeRelicScribe<::ReliquaryOriginRegistrationTestsFixture::Relic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };

    template<>
    class Scribe<::ReliquaryOriginRegistrationTestsFixture::GlobalRelic, BinaryArchive> final
        : public CompositeRelicScribe<::ReliquaryOriginRegistrationTestsFixture::GlobalRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };

    template<>
    class Scribe<::ReliquaryOriginRegistrationTestsFixture::Curator, BinaryArchive> final :
        public CuratorScribe<::ReliquaryOriginRegistrationTestsFixture::Curator, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };
}