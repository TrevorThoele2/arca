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
    class BasicShard
    {
    public:
        std::string myValue;
    public:
        BasicShard() = default;
        explicit BasicShard(std::string myValue);
    };

    struct BasicSignal
    {
        int value = 0;
    };

    class ChildRelic : public TypedRelic
    {
    public:
        int value = 0;
    public:
        ChildRelic() = default;

        [[nodiscard]] RelicStructure Structure() const override { return {}; }
    protected:
        void DoInitialize() override {}
    };

    class ParentRelic : public TypedRelic
    {
    public:
        int value = 0;

        ChildRelicBatch<ChildRelic> children;
    public:
        ParentRelic(int value);

        [[nodiscard]] RelicStructure Structure() const override { return {}; }

        void CreateChild();
    protected:
        void DoInitialize() override;
    };

    class BasicCuratorBase : public Curator
    {
    public:
        bool shouldStart = true;
        static std::function<void(BasicCuratorBase&)> onInitialize;
        std::function<void(BasicCuratorBase&)> onStartStep;
        std::function<void(BasicCuratorBase&)> onWork;
        std::function<void(BasicCuratorBase&)> onStopStep;

        SignalBatch<BasicSignal> basicSignals;

        Reliquary& Owner();
    public:
        explicit BasicCuratorBase(Reliquary& owner);
    protected:
        void InitializeImplementation() override;
        bool StartStepImplementation() override;
        void WorkImplementation() override;
        void StopStepImplementation() override;
    };

    template<size_t differentiator>
    class BasicCurator final : public BasicCuratorBase
    {
    public:
        explicit BasicCurator(Reliquary& owner);
    };
};

template<size_t differentiator>
IntegrationTestsFixture::BasicCurator<differentiator>::BasicCurator(Reliquary& owner) : BasicCuratorBase(owner)
{}

namespace Arca
{
    template<>
    struct ShardTraits<::IntegrationTestsFixture::BasicShard>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::IntegrationTestsFixture::ChildRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::IntegrationTestsFixture::ParentRelic>
    {
        static const TypeHandle typeHandle;
        static std::optional<IntegrationTestsFixture::ParentRelic> Factory(Reliquary& reliquary, int value);
    };

    template<size_t differentiator>
    struct CuratorTraits<::IntegrationTestsFixture::BasicCurator<differentiator>>
    {
        static const TypeHandle typeHandle;
    };

    template<size_t differentiator>
    const TypeHandle CuratorTraits<::IntegrationTestsFixture::BasicCurator<differentiator>>::typeHandle =
        "IntegrationTestsBasicCurator" + ::Chroma::ToString(differentiator);
}

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