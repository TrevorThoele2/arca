#pragma once

#include "GeneralFixture.h"

#include <Arca/Relic.h>
#include <Arca/RelicScribe.h>

#include <Inscription/BinaryArchive.h>

using namespace Arca;

class ReliquaryTestsFixture : public GeneralFixture
{
public:
    ReliquaryTestsFixture();

    class BasicRelic
    {
    public:
        std::string myValue;
    public:
        BasicRelic() = default;
        explicit BasicRelic(std::string myValue);
        explicit BasicRelic(const ::Inscription::BinaryTableData<BasicRelic>& data);
    };

    class StaticRelic
    {
    public:
        std::string myValue;
    public:
        StaticRelic() = default;
        explicit StaticRelic(std::string myValue);
        explicit StaticRelic(const ::Inscription::BinaryTableData<StaticRelic>& data);
    };

    class CustomFactoryRelic
    {
    public:
        std::string myValue;
        std::string factorySupplied;
    public:
        explicit CustomFactoryRelic(std::string factorySupplied);
        CustomFactoryRelic(std::string factorySupplied, std::string myValue);
        explicit CustomFactoryRelic(const ::Inscription::BinaryTableData<CustomFactoryRelic>& data);
    };

    class AbstractRelic
    {
    public:
        std::string abstractValue;
    public:
        explicit AbstractRelic(const std::string& abstractValue);
        virtual ~AbstractRelic() = 0;
        explicit AbstractRelic(const ::Inscription::BinaryTableData<AbstractRelic>& data);
    };

    class DerivedRelic : public AbstractRelic
    {
    public:
        std::string derivedValue;
    public:
        DerivedRelic(const std::string& abstractValue, const std::string& derivedValue);
        explicit DerivedRelic(const ::Inscription::BinaryTableData<DerivedRelic>& data);
    };

    class BasicCurator : public Curator
    {
    public:
        explicit BasicCurator(Reliquary& owner);
    };

    struct BasicSignal
    {};
};

namespace Arca
{
    class CustomFactory final : public RelicFactoryBase
    {
    public:
        using Relic = ::ReliquaryTestsFixture::CustomFactoryRelic;
        using Created = std::optional<Relic>;
    public:
        using RelicFactoryBase::RelicFactoryBase;

        static std::string factorySupplied;

        Created CreateRelic(const std::string& myValue);
    };
}

namespace Arca
{
    template<>
    struct RelicTraits<::ReliquaryTestsFixture::BasicRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::ReliquaryTestsFixture::StaticRelic>
    {
        static const TypeHandle typeHandle;
        static constexpr bool isStatic = true;
    };

    template<>
    struct RelicTraits<::ReliquaryTestsFixture::CustomFactoryRelic>
    {
        static const TypeHandle typeHandle;
        using Factory = CustomFactory;
    };

    template<>
    struct RelicTraits<::ReliquaryTestsFixture::AbstractRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::ReliquaryTestsFixture::DerivedRelic>
    {
        static const TypeHandle typeHandle;
        using Bases = RelicTypeList<::ReliquaryTestsFixture::AbstractRelic>;
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
    struct TableData<::ReliquaryTestsFixture::BasicRelic, BinaryArchive> :
        RelicTableDataBase<::ReliquaryTestsFixture::BasicRelic, BinaryArchive>
    {
        std::string myValue;
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::BasicRelic, BinaryArchive> final :
        public RelicScribe<::ReliquaryTestsFixture::BasicRelic, BinaryArchive>
    {
    public:
        class Table final : public TableBase
        {
        public:
            Table()
            {
                AddDataLink(DataLink::Auto(&ObjectT::myValue, &DataT::myValue));
            }
        };
    };

    template<>
    struct TableData<::ReliquaryTestsFixture::StaticRelic, BinaryArchive> :
        RelicTableDataBase<::ReliquaryTestsFixture::StaticRelic, BinaryArchive>
    {
        std::string myValue;
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::StaticRelic, BinaryArchive> final :
        public RelicScribe<::ReliquaryTestsFixture::StaticRelic, BinaryArchive>
    {
    public:
        class Table final : public TableBase
        {
        public:
            Table()
            {
                AddDataLink(DataLink::Auto(&ObjectT::myValue, &DataT::myValue));
            }
        };
    };

    template<>
    struct TableData<::ReliquaryTestsFixture::CustomFactoryRelic, BinaryArchive> :
        RelicTableDataBase<::ReliquaryTestsFixture::CustomFactoryRelic, BinaryArchive>
    {
        std::string factorySupplied;
        std::string myValue;
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::CustomFactoryRelic, BinaryArchive> final :
        public RelicScribe<::ReliquaryTestsFixture::CustomFactoryRelic, BinaryArchive>
    {
    public:
        class Table final : public TableBase
        {
        public:
            Table()
            {
                MergeDataLinks
                ({
                    DataLink::Auto(&ObjectT::factorySupplied, &DataT::factorySupplied),
                    DataLink::Auto(&ObjectT::myValue, &DataT::myValue) }
                );
            }
        };
    };

    template<>
    struct TableData<::ReliquaryTestsFixture::AbstractRelic, BinaryArchive> :
        RelicTableDataBase<::ReliquaryTestsFixture::AbstractRelic, BinaryArchive>
    {
        std::string abstractValue;
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::AbstractRelic, BinaryArchive> final :
        public RelicScribe<::ReliquaryTestsFixture::AbstractRelic, BinaryArchive>
    {
    public:
        class Table final : public TableBase
        {
        public:
            Table()
            {
                MergeDataLinks
                ({
                    DataLink::Auto(&ObjectT::abstractValue, &DataT::abstractValue) }
                );
            }
        };
    };

    template<>
    struct TableData<::ReliquaryTestsFixture::DerivedRelic, BinaryArchive> :
        RelicTableDataBase<::ReliquaryTestsFixture::DerivedRelic, BinaryArchive>
    {
        std::string derivedValue;
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::DerivedRelic, BinaryArchive> final :
        public RelicScribe<::ReliquaryTestsFixture::DerivedRelic, BinaryArchive>
    {
    public:
        class Table final : public TableBase
        {
        public:
            Table()
            {
                MergeDataLinks
                ({
                    DataLink::Auto(&ObjectT::derivedValue, &DataT::derivedValue) }
                );
            }
        };
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::BasicCurator, BinaryArchive> final :
        public CuratorScribe<::ReliquaryTestsFixture::BasicCurator, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };
}