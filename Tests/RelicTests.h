#pragma once

#include "GeneralFixture.h"

#include <Arca/TypedRelicAutomation.h>
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

    class BasicTypedRelic : public TypedRelicAutomation<BasicTypedRelic, BasicShard>
    {
    public:
        Ptr<BasicShard> basicShard;
    public:
        BasicTypedRelic() = default;
        explicit BasicTypedRelic(const ::Inscription::BinaryTableData<BasicTypedRelic>& data);
    protected:
        void InitializeImplementation() override;
    };

    class GlobalRelic : public TypedRelicAutomation<GlobalRelic, BasicShard>
    {
    public:
        Ptr<BasicShard> basicShard;
    public:
        GlobalRelic() = default;
        explicit GlobalRelic(const ::Inscription::BinaryTableData<GlobalRelic>& data);
    protected:
        void InitializeImplementation() override;
    };

    class MostBasicCustomFactoryRelic : public TypedRelicAutomation<MostBasicCustomFactoryRelic>
    {
    public:
        int value = 0;
    public:
        MostBasicCustomFactoryRelic() = default;
    protected:
        void InitializeImplementation() override {}
    };

    class GuardedCustomFactoryRelic : public TypedRelicAutomation<GuardedCustomFactoryRelic>
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
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::RelicTestsFixture::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::RelicTestsFixture::BasicTypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::RelicTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::RelicTestsFixture::MostBasicCustomFactoryRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
        static std::optional<RelicTestsFixture::MostBasicCustomFactoryRelic>
            Factory(Reliquary& reliquary);
    };

    template<>
    struct Traits<::RelicTestsFixture::GuardedCustomFactoryRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
        static std::optional<RelicTestsFixture::GuardedCustomFactoryRelic>
            Factory(Reliquary& reliquary, int value);
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
    struct TableData<::RelicTestsFixture::GlobalRelic, BinaryArchive> final
        : TableDataBase<::RelicTestsFixture::GlobalRelic, BinaryArchive>
    {
        Base<TypedRelic> base;
    };

    template<>
    class Scribe<::RelicTestsFixture::GlobalRelic, BinaryArchive> final
        : public RelicScribe<::RelicTestsFixture::GlobalRelic, BinaryArchive>
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