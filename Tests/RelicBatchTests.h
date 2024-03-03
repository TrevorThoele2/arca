#pragma once

#include "ReliquaryFixture.h"

#include <Arca/Shard.h>
#include <Arca/TypedRelicAutomation.h>

#include <Inscription/BaseScriven.h>

using namespace Arca;

class RelicBatchFixture : public ReliquaryFixture
{
public:
    class Relic;
    class GlobalRelic;
    class UnregisteredRelic;
    class Shard;
};

class RelicBatchFixture::Relic : public TypedRelicAutomation<Relic>
{
public:
    int value = 0;
public:
    Relic() = default;
    Relic(const ::Inscription::BinaryTableData<Relic>& data);
protected:
    void InitializeImplementation() override;
};

class RelicBatchFixture::GlobalRelic : public TypedRelicAutomation<GlobalRelic>
{
public:
    int value = 0;
public:
    GlobalRelic() = default;
    GlobalRelic(const ::Inscription::BinaryTableData<Relic>& data);
protected:
    void InitializeImplementation() override;
};

class RelicBatchFixture::UnregisteredRelic : public TypedRelicAutomation<UnregisteredRelic>
{
protected:
    void InitializeImplementation() override {}
};

class RelicBatchFixture::Shard
{
public:
    int value = 0;
public:
    Shard() = default;
    explicit Shard(int value);
};

namespace Arca
{
    template<>
    struct Traits<::RelicBatchFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::RelicBatchFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::RelicBatchFixture::UnregisteredRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::RelicBatchFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeHandleName typeName;
    };
}

namespace Inscription
{
    template<>
    struct TableData<::RelicBatchFixture::Relic, BinaryArchive> final
        : TableDataBase<::RelicBatchFixture::Relic, BinaryArchive>
    {
        Base<::Arca::TypedRelic> base;
        int value;
    };

    template<>
    class Scribe<::RelicBatchFixture::Relic, BinaryArchive> final
        : public RelicScribe<::RelicBatchFixture::Relic, BinaryArchive>
    {
    public:
        class Table : public TableBase
        {
        public:
            Table()
            {
                MergeDataLinks({
                    DataLink::Base(data.base),
                    DataLink::Auto(&ObjectT::value, &DataT::value) }
                );
            }
        };
    };

    template<>
    struct TableData<::RelicBatchFixture::GlobalRelic, BinaryArchive> final
        : TableDataBase<::RelicBatchFixture::GlobalRelic, BinaryArchive>
    {
        Base<::Arca::TypedRelic> base;
        int value;
    };

    template<>
    class Scribe<::RelicBatchFixture::GlobalRelic, BinaryArchive> final
        : public RelicScribe<::RelicBatchFixture::Relic, BinaryArchive>
    {
    public:
        class Table : public TableBase
        {
        public:
            Table()
            {
                MergeDataLinks({
                    DataLink::Base(data.base),
                    DataLink::Auto(&ObjectT::value, &DataT::value) }
                );
            }
        };
    };

    template<>
    class Scribe<::RelicBatchFixture::Shard, BinaryArchive> final
        : public ShardScribe<::RelicBatchFixture::Shard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.value);
        }
    };
}