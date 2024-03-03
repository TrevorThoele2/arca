#pragma once

#include "GeneralFixture.h"

#include <Arca/TypedVessel.h>

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

    class OtherRelic
    {
    public:
        int myValue;
    public:
        OtherRelic() = default;
        explicit OtherRelic(int myValue);
        explicit OtherRelic(const ::Inscription::BinaryTableData<OtherRelic> & data);
    };

    class BasicTypedVessel : public TypedVessel<BasicRelic>
    {
    public:
        BasicRelic* basicRelic = nullptr;
    public:
        BasicTypedVessel(VesselID id, Reliquary& owner);
        explicit BasicTypedVessel(const ::Inscription::BinaryTableData<BasicTypedVessel>& data);
    private:
        void Setup();
    };

    class StaticVessel : public TypedVessel<BasicRelic>
    {
    public:
        BasicRelic* basicRelic = nullptr;
    public:
        StaticVessel(VesselID id, Reliquary& owner);
        explicit StaticVessel(const ::Inscription::BinaryTableData<StaticVessel>& data);
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
    struct RelicTraits<::ReliquaryTestsFixture::BasicRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::ReliquaryTestsFixture::OtherRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct VesselTraits<::ReliquaryTestsFixture::BasicTypedVessel>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct VesselTraits<::ReliquaryTestsFixture::StaticVessel>
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
    struct TableData<::ReliquaryTestsFixture::BasicRelic, BinaryArchive> :
        TableDataBase<::ReliquaryTestsFixture::BasicRelic, BinaryArchive>
    {
        std::string myValue;
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::BasicRelic, BinaryArchive> final :
        public TableScribe<::ReliquaryTestsFixture::BasicRelic, BinaryArchive>
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
    struct TableData<::ReliquaryTestsFixture::OtherRelic, BinaryArchive> :
        TableDataBase<::ReliquaryTestsFixture::OtherRelic, BinaryArchive>
    {
        int myValue;
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::OtherRelic, BinaryArchive> final :
        public TableScribe<::ReliquaryTestsFixture::OtherRelic, BinaryArchive>
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
    struct TableData<::ReliquaryTestsFixture::BasicTypedVessel, BinaryArchive> :
        TableDataBase<::ReliquaryTestsFixture::BasicTypedVessel, BinaryArchive>
    {
        Base<TypedVessel<::ReliquaryTestsFixture::BasicRelic>> base;
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::BasicTypedVessel, BinaryArchive> final :
        public TableScribe<::ReliquaryTestsFixture::BasicTypedVessel, BinaryArchive>
    {
    public:
        class Table final : public TableBase
        {
        public:
            Table()
            {
                AddDataLink(DataLink::Base(Type<TypedVessel<::ReliquaryTestsFixture::BasicRelic>>{}));
            }
        };
    };

    template<>
    struct TableData<::ReliquaryTestsFixture::StaticVessel, BinaryArchive> :
        TableDataBase<::ReliquaryTestsFixture::StaticVessel, BinaryArchive>
    {
        Base<TypedVessel<::ReliquaryTestsFixture::BasicRelic>> base;
        std::string myValue;
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::StaticVessel, BinaryArchive> final :
        public TableScribe<::ReliquaryTestsFixture::StaticVessel, BinaryArchive>
    {
    public:
        class Table final : public TableBase
        {
        public:
            Table()
            {
                AddDataLink(DataLink::Base(Type<TypedVessel<::ReliquaryTestsFixture::BasicRelic>>{}));
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