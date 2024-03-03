#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelicAutomation.h>
#include <Arca/Shard.h>

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
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::ReliquaryRegistrationTestsFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::ReliquaryRegistrationTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::ReliquaryRegistrationTestsFixture::Curator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::ReliquaryRegistrationTestsFixture::Signal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static const TypeHandleName typeName;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::ReliquaryRegistrationTestsFixture::Shard, BinaryArchive> final
        : public ShardScribe<::ReliquaryRegistrationTestsFixture::Shard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };

    template<>
    class Scribe<::ReliquaryRegistrationTestsFixture::Relic, BinaryArchive> final
        : public CompositeRelicScribe<::ReliquaryRegistrationTestsFixture::Relic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };

    template<>
    class Scribe<::ReliquaryRegistrationTestsFixture::GlobalRelic, BinaryArchive> final
        : public CompositeRelicScribe<::ReliquaryRegistrationTestsFixture::GlobalRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };

    template<>
    class Scribe<::ReliquaryRegistrationTestsFixture::Curator, BinaryArchive> final :
        public CuratorScribe<::ReliquaryRegistrationTestsFixture::Curator, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };
}