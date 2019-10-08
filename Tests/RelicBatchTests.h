#pragma once

#include "ReliquaryFixture.h"

#include <Arca/Relic.h>
#include <Arca/RelicScribe.h>

using namespace Arca;

class RelicBatchFixture : public ReliquaryFixture
{
public:
    RelicBatchFixture();

    class AbstractRelic;
    class DerivedRelic;
    class UnregisteredRelic;
};

class RelicBatchFixture::AbstractRelic
{
public:
    std::string abstractValue;
public:
    virtual ~AbstractRelic() = 0;
protected:
    AbstractRelic() = default;
    AbstractRelic(const std::string& abstractValue);
    AbstractRelic(const ::Inscription::BinaryTableData<AbstractRelic>& data);
};

class RelicBatchFixture::DerivedRelic : public AbstractRelic
{
public:
    int derivedValue = 0;
public:
    DerivedRelic() = default;
    DerivedRelic(const std::string& abstractValue, int derivedValue);
    DerivedRelic(const ::Inscription::BinaryTableData<DerivedRelic>& data);
};

class RelicBatchFixture::UnregisteredRelic
{};

namespace Arca
{
    template<>
    struct RelicTraits<::RelicBatchFixture::AbstractRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::RelicBatchFixture::DerivedRelic>
    {
        static const TypeHandle typeHandle;
        using Bases = RelicTypeList<::RelicBatchFixture::AbstractRelic>;
    };

    template<>
    struct RelicTraits<::RelicBatchFixture::UnregisteredRelic>
    {
        static const TypeHandle typeHandle;
    };
}

namespace Inscription
{
    template<>
    struct TableData<::RelicBatchFixture::AbstractRelic, BinaryArchive> :
        RelicTableDataBase<::RelicBatchFixture::AbstractRelic, BinaryArchive>
    {
        std::string abstractValue;
    };

    template<>
    class Scribe<::RelicBatchFixture::AbstractRelic, BinaryArchive> final :
        public RelicScribe<::RelicBatchFixture::AbstractRelic, BinaryArchive>
    {
    public:
        class Table final : public TableBase
        {
        public:
            Table();
        };
    };

    template<>
    struct TableData<::RelicBatchFixture::DerivedRelic, BinaryArchive> final :
        RelicTableDataBase<::RelicBatchFixture::DerivedRelic, BinaryArchive>
    {
        int derivedValue = 0;
    };

    template<>
    class Scribe<::RelicBatchFixture::DerivedRelic, BinaryArchive> final :
        public RelicScribe<::RelicBatchFixture::DerivedRelic, BinaryArchive>
    {
    public:
        class Table final : public TableBase
        {
        public:
            Table();
        };
    };
}