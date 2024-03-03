#pragma once

#include "GeneralFixture.h"

#include <Arca/TypedVessel.h>

using namespace Arca;

class VesselTestsFixture : public GeneralFixture
{
public:
    class BasicRelic
    {
    public:
        std::string myValue;
    public:
        BasicRelic() = default;
        explicit BasicRelic(std::string myValue);
    };

    class OtherRelic
    {
    public:
        int myValue;
    public:
        OtherRelic() = default;
        explicit OtherRelic(int myValue);
        explicit OtherRelic(const ::Inscription::BinaryTableData<OtherRelic>& data);
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
};

namespace Arca
{
    template<>
    struct RelicTraits<::VesselTestsFixture::BasicRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::VesselTestsFixture::OtherRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct VesselTraits<::VesselTestsFixture::BasicTypedVessel>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct VesselTraits<::VesselTestsFixture::StaticVessel>
    {
        static const TypeHandle typeHandle;
    };
}

namespace Inscription
{
    template<>
    struct TableData<VesselTestsFixture::OtherRelic, BinaryArchive> :
        TableDataBase<VesselTestsFixture::OtherRelic, BinaryArchive>
    {
        int myValue;
    };

    template<>
    class Scribe<VesselTestsFixture::OtherRelic, BinaryArchive> final :
        public TableScribe<VesselTestsFixture::OtherRelic, BinaryArchive>
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
    struct TableData<VesselTestsFixture::BasicTypedVessel, BinaryArchive> :
        TableDataBase<VesselTestsFixture::BasicTypedVessel, BinaryArchive>
    {
        Base<TypedVessel<VesselTestsFixture::BasicRelic>> base;
    };

    template<>
    class Scribe<VesselTestsFixture::BasicTypedVessel, BinaryArchive> final :
        public TableScribe<VesselTestsFixture::BasicTypedVessel, BinaryArchive>
    {
    public:
        class Table final : public TableBase
        {
        public:
            Table()
            {
                AddDataLink(DataLink::Base(Type<TypedVessel<::VesselTestsFixture::BasicRelic>>{}));
            }
        };
    };

    template<>
    struct TableData<VesselTestsFixture::StaticVessel, BinaryArchive> :
        TableDataBase<VesselTestsFixture::StaticVessel, BinaryArchive>
    {
        Base<TypedVessel<::VesselTestsFixture::BasicRelic>> base;
        std::string myValue;
    };

    template<>
    class Scribe<VesselTestsFixture::StaticVessel, BinaryArchive> final :
        public TableScribe<VesselTestsFixture::StaticVessel, BinaryArchive>
    {
    public:
        class Table final : public TableBase
        {
        public:
            Table()
            {
                AddDataLink(DataLink::Base(Type<TypedVessel<::VesselTestsFixture::BasicRelic>>{}));
            }
        };
    };
}