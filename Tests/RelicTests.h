#pragma once

#include "GeneralFixture.h"

#include <Arca/TypedRelicWithShards.h>
#include <Arca/Shard.h>

#include <Arca/Serialization.h>

using namespace Arca;

class RelicTestsFixture : public GeneralFixture
{
public:
    class BasicShard
    {
    public:
        std::string myValue;
    public:
        BasicShard() = default;
        explicit BasicShard(std::string myValue);
    };

    class OtherShard
    {
    public:
        int myValue;
    public:
        OtherShard() = default;
        explicit OtherShard(int myValue);
    };

    class BasicTypedRelic : public TypedRelicWithShards<BasicShard>
    {
    public:
        BasicShard* basicShard = nullptr;
    public:
        BasicTypedRelic(RelicID id, Reliquary& owner);
        BasicTypedRelic(const ::Inscription::BinaryTableData<BasicTypedRelic>& data);
    private:
        void Setup();
    };

    class StaticRelic : public TypedRelicWithShards<BasicShard>
    {
    public:
        BasicShard* basicShard = nullptr;
    public:
        StaticRelic(RelicID id, Reliquary& owner);
        StaticRelic(const ::Inscription::BinaryTableData<StaticRelic>& data);
    private:
        void Setup();
    };
};

namespace Arca
{
    template<>
    struct ShardTraits<::RelicTestsFixture::BasicShard>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct ShardTraits<::RelicTestsFixture::OtherShard>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::RelicTestsFixture::BasicTypedRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::RelicTestsFixture::StaticRelic>
    {
        static const TypeHandle typeHandle;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::RelicTestsFixture::BasicShard, BinaryArchive> final
        : public ShardScribe<::RelicTestsFixture::BasicShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::RelicTestsFixture::OtherShard, BinaryArchive> final
        : public ShardScribe<::RelicTestsFixture::OtherShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    struct TableData<::RelicTestsFixture::BasicTypedRelic, BinaryArchive> final
        : TableDataBase<::RelicTestsFixture::BasicTypedRelic, BinaryArchive>
    {
        Base<TypedRelicWithShards<::RelicTestsFixture::BasicShard>> base;
    };

    template<>
    class Scribe<::RelicTestsFixture::BasicTypedRelic, BinaryArchive> final
        : TableScribe<::RelicTestsFixture::BasicTypedRelic, BinaryArchive>
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
    struct TableData<::RelicTestsFixture::StaticRelic, BinaryArchive> final
        : TableDataBase<::RelicTestsFixture::StaticRelic, BinaryArchive>
    {
        Base<TypedRelicWithShards<::RelicTestsFixture::BasicShard>> base;
    };

    template<>
    class Scribe<::RelicTestsFixture::StaticRelic, BinaryArchive> final
        : TableScribe<::RelicTestsFixture::StaticRelic, BinaryArchive>
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