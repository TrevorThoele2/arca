#pragma once

#include "ReliquaryFixture.h"

#include <Arca/Shard.h>
#include <Arca/TypedRelic.h>

#include <Inscription/BaseScriven.h>

using namespace Arca;

class RelicBatchFixture : public ReliquaryFixture
{
public:
    class Relic;
    class StaticRelic;
    class UnregisteredRelic;
    class Shard;
};

class RelicBatchFixture::Relic : public TypedRelic
{
public:
    int value = 0;
public:
    Relic(RelicID id, Reliquary& owner);
    Relic(const ::Inscription::BinaryTableData<Relic>& data);
};

class RelicBatchFixture::StaticRelic : public TypedRelic
{
public:
    int value = 0;
public:
    StaticRelic(RelicID id, Reliquary& owner);
    StaticRelic(const ::Inscription::BinaryTableData<Relic>& data);
};

class RelicBatchFixture::UnregisteredRelic : public TypedRelic
{};

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
    struct RelicTraits<::RelicBatchFixture::Relic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::RelicBatchFixture::StaticRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::RelicBatchFixture::UnregisteredRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct ShardTraits<::RelicBatchFixture::Shard>
    {
        static const TypeHandle typeHandle;
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
    struct TableData<::RelicBatchFixture::StaticRelic, BinaryArchive> final
        : TableDataBase<::RelicBatchFixture::StaticRelic, BinaryArchive>
    {
        Base<::Arca::TypedRelic> base;
        int value;
    };

    template<>
    class Scribe<::RelicBatchFixture::StaticRelic, BinaryArchive> final
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