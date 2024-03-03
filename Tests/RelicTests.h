#pragma once

#include "GeneralFixture.h"

#include <Arca/TypedRelic.h>
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

    class BasicTypedRelic : public TypedRelic
    {
    public:
        BasicShard* basicShard = nullptr;
    public:
        BasicTypedRelic() = default;
        explicit BasicTypedRelic(const ::Inscription::BinaryTableData<BasicTypedRelic>& data);

        void Initialize(Reliquary& reliquary) override;
        [[nodiscard]] RelicStructure Structure() const override;
    private:
        using Shards = ::Chroma::VariadicTemplate<BasicShard>;
    };

    class StaticRelic : public TypedRelic
    {
    public:
        BasicShard* basicShard = nullptr;
    public:
        StaticRelic() = default;
        explicit StaticRelic(const ::Inscription::BinaryTableData<StaticRelic>& data);

        void Initialize(Reliquary& reliquary) override;
        [[nodiscard]] RelicStructure Structure() const override;
    private:
        using Shards = ::Chroma::VariadicTemplate<BasicShard>;
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
        Base<TypedRelic> base;
    };

    template<>
    class Scribe<::RelicTestsFixture::BasicTypedRelic, BinaryArchive> final
        : public RelicScribe<::RelicTestsFixture::BasicTypedRelic, BinaryArchive>
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
        Base<TypedRelic> base;
    };

    template<>
    class Scribe<::RelicTestsFixture::StaticRelic, BinaryArchive> final
        : public RelicScribe<::RelicTestsFixture::StaticRelic, BinaryArchive>
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