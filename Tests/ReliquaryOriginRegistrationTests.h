#pragma once

#include "GeneralFixture.h"

#include <Arca/TypedRelic.h>
#include <Arca/Shard.h>

#include <Inscription/BinaryArchive.h>
#include "Inscription/BaseScriven.h"

using namespace Arca;

class ReliquaryOriginRegistrationTestsFixture : public GeneralFixture
{
public:
    class Shard
    {};

    class Relic : public TypedRelic
    {};

    class StaticRelic : public TypedRelic
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
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::StaticRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
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
    struct TableData<::ReliquaryOriginRegistrationTestsFixture::Relic, BinaryArchive> final
        : TableDataBase<::ReliquaryOriginRegistrationTestsFixture::Relic, BinaryArchive>
    {
        Base<TypedRelic> base;
    };

    template<>
    class Scribe<::ReliquaryOriginRegistrationTestsFixture::Relic, BinaryArchive> final
        : public RelicScribe<::ReliquaryOriginRegistrationTestsFixture::Relic, BinaryArchive>
    {
    public:
        class Table : public TableBase
        {
        public:
            Table()
            {
                AddDataLink(DataLink::Base(data.base));
            }
        };
    };

    template<>
    struct TableData<::ReliquaryOriginRegistrationTestsFixture::StaticRelic, BinaryArchive> final
        : TableDataBase<::ReliquaryOriginRegistrationTestsFixture::StaticRelic, BinaryArchive>
    {
        Base<TypedRelic> base;
    };

    template<>
    class Scribe<::ReliquaryOriginRegistrationTestsFixture::StaticRelic, BinaryArchive> final
        : public RelicScribe<::ReliquaryOriginRegistrationTestsFixture::StaticRelic, BinaryArchive>
    {
    public:
        class Table : public TableBase
        {
        public:
            Table()
            {
                AddDataLink(DataLink::Base(data.base));
            }
        };
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