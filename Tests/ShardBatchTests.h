#pragma once

#include "ReliquaryFixture.h"

#include <Arca/Shard.h>
#include <Arca/TypedRelicAutomation.h>

using namespace Arca;

class ShardBatchFixture : public ReliquaryFixture
{
public:
    class Shard;
    class UnregisteredShard;
    class GlobalRelic;
};

class ShardBatchFixture::Shard
{
public:
    int value = 0;
public:
    Shard() = default;
    explicit Shard(int value);
};

class ShardBatchFixture::UnregisteredShard
{};

class ShardBatchFixture::GlobalRelic : public TypedRelicAutomation<GlobalRelic, Shard>
{
public:
    Ptr<Shard> shard;
protected:
    void InitializeImplementation() override;
};

namespace Arca
{
    template<>
    struct Traits<::ShardBatchFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::ShardBatchFixture::UnregisteredShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::ShardBatchFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::ShardBatchFixture::Shard, BinaryArchive> final
        : public ShardScribe<::ShardBatchFixture::Shard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.value);
        }
    };

    template<>
    struct TableData<::ShardBatchFixture::GlobalRelic, BinaryArchive> final
        : TableDataBase<::ShardBatchFixture::GlobalRelic, BinaryArchive>
    {};

    template<>
    class Scribe<::ShardBatchFixture::GlobalRelic, BinaryArchive> final
        : public RelicScribe<::ShardBatchFixture::GlobalRelic, BinaryArchive>
    {
    public:
        class Table : public TableBase
        {};
    };
}