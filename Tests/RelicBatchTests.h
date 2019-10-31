#pragma once

#include "ReliquaryFixture.h"

#include <Arca/Relic.h>

using namespace Arca;

class RelicBatchFixture : public ReliquaryFixture
{
public:
    RelicBatchFixture();

    class Relic;
    class UnregisteredRelic;
};

class RelicBatchFixture::Relic
{
public:
    int value = 0;
public:
    Relic() = default;
    Relic(int value);
    Relic(const ::Inscription::BinaryTableData<Relic>& data);
};

class RelicBatchFixture::UnregisteredRelic
{};

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
}