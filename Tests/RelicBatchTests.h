#pragma once

#include "ReliquaryFixture.h"

#include <Arca/Relic.h>
#include <Arca/TypedVessel.h>

using namespace Arca;

class RelicBatchFixture : public ReliquaryFixture
{
public:
    RelicBatchFixture();

    class Relic;
    class UnregisteredRelic;
    class StaticVessel;
};

class RelicBatchFixture::Relic
{
public:
    int value = 0;
public:
    Relic() = default;
    explicit Relic(int value);
    explicit Relic(const ::Inscription::BinaryTableData<Relic>& data);
};

class RelicBatchFixture::UnregisteredRelic
{};

class RelicBatchFixture::StaticVessel : public TypedVessel<Relic>
{
public:
    Relic* relic = nullptr;
public:
    StaticVessel(VesselID id, Reliquary& owner);
    explicit StaticVessel(const ::Inscription::BinaryTableData<StaticVessel>& data);
private:
    void Setup();
};

namespace Arca
{
    template<>
    struct RelicTraits<::RelicBatchFixture::Relic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::RelicBatchFixture::UnregisteredRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct VesselTraits<::RelicBatchFixture::StaticVessel>
    {
        static const TypeHandle typeHandle;
    };
}

namespace Inscription
{
    template<>
    struct TableData<::RelicBatchFixture::Relic, BinaryArchive> final :
        TableDataBase<::RelicBatchFixture::Relic, BinaryArchive>
    {
        int value = 0;
    };

    template<>
    class Scribe<::RelicBatchFixture::Relic, BinaryArchive> final :
        public TableScribe<::RelicBatchFixture::Relic, BinaryArchive>
    {
    public:
        class Table final : public TableBase
        {
        public:
            Table();
        };
    };

    template<>
    struct TableData<::RelicBatchFixture::StaticVessel, BinaryArchive> final :
        TableDataBase<::RelicBatchFixture::StaticVessel, BinaryArchive>
    {
        Base<TypedVessel<::RelicBatchFixture::Relic>> base;
    };

    template<>
    class Scribe<::RelicBatchFixture::StaticVessel, BinaryArchive> final :
        public TableScribe<::RelicBatchFixture::StaticVessel, BinaryArchive>
    {
    public:
        class Table final : public TableBase
        {
        public:
            Table();
        };
    };
}