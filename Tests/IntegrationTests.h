#pragma once

#include "GeneralFixture.h"

#include <Arca/TypedRelic.h>
#include <Arca/Shard.h>

#include <Inscription/BinaryArchive.h>
#include "Inscription/BaseScriven.h"

#include <Chroma/StringUtility.h>

using namespace Arca;

class IntegrationTestsFixture : public GeneralFixture
{
public:
    class BasicShard;
    struct BasicSignal;
    class ChildRelic;
    class ParentRelic;
    class BasicCuratorBase;
    template<size_t differentiator>
    class BasicCurator;
};

namespace Arca
{
    template<>
    struct Traits<::IntegrationTestsFixture::BasicShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::IntegrationTestsFixture::BasicSignal>
    {
        static const ObjectType objectType = ObjectType::Signal;
    };

    template<>
    struct Traits<::IntegrationTestsFixture::ChildRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
    };

    template<>
    struct Traits<::IntegrationTestsFixture::ParentRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeHandleName typeName;
        static std::optional<IntegrationTestsFixture::ParentRelic> Factory(Reliquary& reliquary, int value);
    };

    template<size_t differentiator>
    struct Traits<::IntegrationTestsFixture::BasicCurator<differentiator>>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static const TypeHandleName typeName;
    };

    template<size_t differentiator>
    const TypeHandleName Traits<::IntegrationTestsFixture::BasicCurator<differentiator>>::typeName =
        "IntegrationTestsBasicCurator" + ::Chroma::ToString(differentiator);
}

class IntegrationTestsFixture::BasicShard
{
public:
    std::string myValue;
public:
    BasicShard() = default;
    explicit BasicShard(std::string myValue);
};

struct IntegrationTestsFixture::BasicSignal
{
    int value = 0;
};

class IntegrationTestsFixture::ChildRelic : public TypedRelic
{
public:
    int value = 0;
public:
    ChildRelic() = default;
protected:
    void InitializeImplementation() override {}
};

class IntegrationTestsFixture::ParentRelic : public TypedRelic
{
public:
    int value = 0;
public:
    ParentRelic(int value);

    void CreateChild();
};

class IntegrationTestsFixture::BasicCuratorBase : public Curator
{
public:
    bool shouldStart = true;
    static std::function<void(BasicCuratorBase&)> onInitialize;
    std::function<void(BasicCuratorBase&)> onStartStep;
    std::function<void(BasicCuratorBase&)> onWork;
    std::function<void(BasicCuratorBase&)> onStopStep;

    Batch<BasicSignal> basicSignals;

    Reliquary& Owner();
public:
    BasicCuratorBase();
protected:
    void InitializeImplementation() override;
    bool StartStepImplementation() override;
    void WorkImplementation() override;
    void StopStepImplementation() override;
};

template<size_t differentiator>
class IntegrationTestsFixture::BasicCurator final : public BasicCuratorBase
{
public:
    BasicCurator() = default;
};

namespace Inscription
{
    template<>
    class Scribe<::IntegrationTestsFixture::BasicShard, BinaryArchive> final
        : public ShardScribe<::IntegrationTestsFixture::BasicShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    struct TableData<::IntegrationTestsFixture::ChildRelic, BinaryArchive> final
        : TableDataBase<::IntegrationTestsFixture::ChildRelic, BinaryArchive>
    {
        Base<TypedRelic> base;
    };

    template<>
    class Scribe<::IntegrationTestsFixture::ChildRelic, BinaryArchive> final
        : public RelicScribe<::IntegrationTestsFixture::ChildRelic, BinaryArchive>
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
    struct TableData<::IntegrationTestsFixture::ParentRelic, BinaryArchive> final
        : TableDataBase<::IntegrationTestsFixture::ParentRelic, BinaryArchive>
    {
        Base<TypedRelic> base;
    };

    template<>
    class Scribe<::IntegrationTestsFixture::ParentRelic, BinaryArchive> final
        : public RelicScribe<::IntegrationTestsFixture::ParentRelic, BinaryArchive>
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

    template<size_t differentiator>
    class Scribe<::IntegrationTestsFixture::BasicCurator<differentiator>, BinaryArchive> final :
        public CuratorScribe<::IntegrationTestsFixture::BasicCurator<differentiator>, BinaryArchive>
    {
    private:
        using BaseT = CuratorScribe<::IntegrationTestsFixture::BasicCurator<differentiator>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;

        using BaseT::Scriven;
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };
}