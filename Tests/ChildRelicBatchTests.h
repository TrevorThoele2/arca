#pragma once

#include "ReliquaryFixture.h"

#include <Arca/Shard.h>

class ChildRelicBatchTestsFixture : public ReliquaryFixture
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

    class BasicTypedRelic : public TypedRelic
    {
    public:
        BasicShard* basicShard = nullptr;
    public:
        BasicTypedRelic() = default;
        explicit BasicTypedRelic(const ::Inscription::BinaryTableData<BasicTypedRelic>& data);

        [[nodiscard]] RelicStructure Structure() const override;
    protected:
        void DoInitialize() override;
    private:
        using Shards = ::Chroma::VariadicTemplate<BasicShard>;
    };
};

namespace Arca
{
    template<>
    struct ShardTraits<::ChildRelicBatchTestsFixture::BasicShard>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::ChildRelicBatchTestsFixture::BasicTypedRelic>
    {
        static const TypeHandle typeHandle;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::ChildRelicBatchTestsFixture::BasicShard, BinaryArchive> final
        : public ShardScribe<::ChildRelicBatchTestsFixture::BasicShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    struct TableData<::ChildRelicBatchTestsFixture::BasicTypedRelic, BinaryArchive> final
        : TableDataBase<::ChildRelicBatchTestsFixture::BasicTypedRelic, BinaryArchive>
    {
        Base<TypedRelic> base;
    };

    template<>
    class Scribe<::ChildRelicBatchTestsFixture::BasicTypedRelic, BinaryArchive> final
        : public RelicScribe<::ChildRelicBatchTestsFixture::BasicTypedRelic, BinaryArchive>
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