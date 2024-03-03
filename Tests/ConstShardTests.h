#pragma once

#include "ReliquaryFixture.h"

#include <Arca/Shard.h>
#include <Arca/TypedRelic.h>

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

class ConstShardTestsFixture::Relic : public TypedRelic
{
public:
    const Shard* shard = nullptr;
protected:
    void InitializeImplementation() override;
};

namespace Arca
{
    template<>
    struct Traits<::ConstShardTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeHandle typeHandle;
    };

    template<>
    struct Traits<::ConstShardTestsFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandle typeHandle;
        using Shards = ShardList<const ConstShardTestsFixture::Shard>;
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