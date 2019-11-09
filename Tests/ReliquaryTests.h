#pragma once

#include "GeneralFixture.h"

#include <Arca/TypedRelicWithShards.h>
#include <Arca/Shard.h>

#include <Inscription/BinaryArchive.h>
#include "Inscription/BaseScriven.h"

using namespace Arca;

class ReliquaryTestsFixture : public GeneralFixture
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

    class OtherBasicShard
    {
    public:
        std::string myValue;
    public:
        OtherBasicShard() = default;
        explicit OtherBasicShard(int myValue);
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
        explicit BasicTypedRelic(const ::Inscription::BinaryTableData<BasicTypedRelic>& data);
    private:
        void Setup();
    };

    class StaticRelic : public TypedRelicWithShards<BasicShard>
    {
    public:
        BasicShard* basicShard = nullptr;
    public:
        StaticRelic(RelicID id, Reliquary& owner);
        explicit StaticRelic(const ::Inscription::BinaryTableData<StaticRelic>& data);
    private:
        void Setup();
    };

    class BasicCurator final : public Curator
    {
    public:
        explicit BasicCurator(Reliquary& owner);
    };

    struct BasicSignal
    {};
};

namespace Arca
{
    template<>
    struct ShardTraits<::ReliquaryTestsFixture::BasicShard>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct ShardTraits<::ReliquaryTestsFixture::OtherBasicShard>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct ShardTraits<::ReliquaryTestsFixture::OtherShard>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::ReliquaryTestsFixture::BasicTypedRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::ReliquaryTestsFixture::StaticRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct CuratorTraits<::ReliquaryTestsFixture::BasicCurator>
    {
        static const TypeHandle typeHandle;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::ReliquaryTestsFixture::BasicShard, BinaryArchive> final
        : public ShardScribe<::ReliquaryTestsFixture::BasicShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::OtherBasicShard, BinaryArchive> final
        : public ShardScribe<::ReliquaryTestsFixture::OtherBasicShard, BinaryArchive>
    {
    public:
        static std::vector<TypeHandle> InputTypeHandles(const ArchiveT& archive)
        {
            return { "ReliquaryTestsBasicShard" };
        }
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::OtherShard, BinaryArchive> final
        : public ShardScribe<::ReliquaryTestsFixture::OtherShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    struct TableData<::ReliquaryTestsFixture::BasicTypedRelic, BinaryArchive> final
        : TableDataBase<::ReliquaryTestsFixture::BasicTypedRelic, BinaryArchive>
    {
        Base<TypedRelicWithShards<ReliquaryTestsFixture::BasicShard>> base;
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::BasicTypedRelic, BinaryArchive> final
        : public RelicScribe<::ReliquaryTestsFixture::BasicTypedRelic, BinaryArchive>
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
    struct TableData<::ReliquaryTestsFixture::StaticRelic, BinaryArchive> final
        : TableDataBase<::ReliquaryTestsFixture::StaticRelic, BinaryArchive>
    {
        Base<TypedRelicWithShards<ReliquaryTestsFixture::BasicShard>> base;
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::StaticRelic, BinaryArchive> final
        : RelicScribe<::ReliquaryTestsFixture::StaticRelic, BinaryArchive>
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
    class Scribe<::ReliquaryTestsFixture::BasicCurator, BinaryArchive> final :
        public CuratorScribe<::ReliquaryTestsFixture::BasicCurator, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };
}