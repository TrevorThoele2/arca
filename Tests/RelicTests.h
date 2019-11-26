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
    protected:
        void InitializeImplementation() override;
    };

    class StaticRelic : public TypedRelic
    {
    public:
        BasicShard* basicShard = nullptr;
    public:
        StaticRelic() = default;
        explicit StaticRelic(const ::Inscription::BinaryTableData<StaticRelic>& data);
    protected:
        void InitializeImplementation() override;
    };

    class MostBasicCustomFactoryRelic : public TypedRelic
    {
    public:
        int value = 0;
    public:
        MostBasicCustomFactoryRelic() = default;
    protected:
        void InitializeImplementation() override {}
    };

    class GuardedCustomFactoryRelic : public TypedRelic
    {
    public:
        int value = 0;
    public:
        GuardedCustomFactoryRelic() = default;
    protected:
        void InitializeImplementation() override {}
    };
};

namespace Arca
{
    template<>
    struct Traits<::RelicTestsFixture::BasicShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeHandle typeHandle;
    };

    template<>
    struct Traits<::RelicTestsFixture::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeHandle typeHandle;
    };

    template<>
    struct Traits<::RelicTestsFixture::BasicTypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandle typeHandle;
        using Shards = ShardList<::RelicTestsFixture::BasicShard>;
    };

    template<>
    struct Traits<::RelicTestsFixture::StaticRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandle typeHandle;
        using Shards = ShardList<::RelicTestsFixture::BasicShard>;
    };

    template<>
    struct Traits<::RelicTestsFixture::MostBasicCustomFactoryRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandle typeHandle;
        static std::optional<RelicTestsFixture::MostBasicCustomFactoryRelic> Factory(Reliquary& reliquary);
    };

    template<>
    struct Traits<::RelicTestsFixture::GuardedCustomFactoryRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandle typeHandle;
        static std::optional<RelicTestsFixture::GuardedCustomFactoryRelic> Factory(Reliquary& reliquary, int value);
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

    template<>
    struct TableData<::RelicTestsFixture::MostBasicCustomFactoryRelic, BinaryArchive> final
        : TableDataBase<::RelicTestsFixture::MostBasicCustomFactoryRelic, BinaryArchive>
    {
        Base<TypedRelic> base;
    };

    template<>
    class Scribe<::RelicTestsFixture::MostBasicCustomFactoryRelic, BinaryArchive> final
        : public RelicScribe<::RelicTestsFixture::MostBasicCustomFactoryRelic, BinaryArchive>
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
    struct TableData<::RelicTestsFixture::GuardedCustomFactoryRelic, BinaryArchive> final
        : TableDataBase<::RelicTestsFixture::GuardedCustomFactoryRelic, BinaryArchive>
    {
        Base<TypedRelic> base;
    };

    template<>
    class Scribe<::RelicTestsFixture::GuardedCustomFactoryRelic, BinaryArchive> final
        : public RelicScribe<::RelicTestsFixture::GuardedCustomFactoryRelic, BinaryArchive>
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