#pragma once

#include "ReliquaryFixture.h"

#include <Arca/Shard.h>
#include <Arca/TypedRelicAutomation.h>

using namespace Arca;

class ConstShardTestsFixture : public ReliquaryFixture
{
public:
    class Shard;
    class Relic;
};

class ConstShardTestsFixture::Shard
{
public:
    int value = 100;
public:
    Shard() = default;
    explicit Shard(int value);
};

class ConstShardTestsFixture::Relic : public TypedRelicAutomation<Relic, const Shard>
{
public:
    Ptr<const Shard> shard;
protected:
    void InitializeImplementation() override;
};

namespace Arca
{
    template<>
    struct Traits<::ConstShardTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::ConstShardTestsFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::ConstShardTestsFixture::Shard, BinaryArchive> final
        : public ShardScribe<::ConstShardTestsFixture::Shard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.value);
        }
    };

    template<>
    struct TableData<::ConstShardTestsFixture::Relic, BinaryArchive> final
        : TableDataBase<::ConstShardTestsFixture::Relic, BinaryArchive>
    {
        Base<TypedRelic> base;
    };

    template<>
    class Scribe<::ConstShardTestsFixture::Relic, BinaryArchive> final
        : public RelicScribe<::ConstShardTestsFixture::Relic, BinaryArchive>
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
}