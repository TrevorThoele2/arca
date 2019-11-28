#pragma once

#include "ReliquaryFixture.h"

#include <Arca/Shard.h>
#include <Arca/TypedRelic.h>

using namespace Arca;

class IsTypeTestsFixture : public ReliquaryFixture
{
public:
    class Shard;
    class Relic;
    class Curator;
};

class IsTypeTestsFixture::Shard
{};

class IsTypeTestsFixture::Relic : public TypedRelic
{};

class IsTypeTestsFixture::Curator : public Arca::Curator
{};

namespace Arca
{
    template<>
    struct Traits<::IsTypeTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::IsTypeTestsFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::IsTypeTestsFixture::Curator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static const TypeHandleName typeName;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::IsTypeTestsFixture::Shard, BinaryArchive> final
        : public ShardScribe<::IsTypeTestsFixture::Shard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };

    template<>
    struct TableData<::IsTypeTestsFixture::Relic, BinaryArchive> final
        : TableDataBase<::IsTypeTestsFixture::Relic, BinaryArchive>
    {
        Base<TypedRelic> base;
    };

    template<>
    class Scribe<::IsTypeTestsFixture::Relic, BinaryArchive> final
        : public RelicScribe<::IsTypeTestsFixture::Relic, BinaryArchive>
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
    class Scribe<::IsTypeTestsFixture::Curator, BinaryArchive> final
        : public ShardScribe<::IsTypeTestsFixture::Curator, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };
}