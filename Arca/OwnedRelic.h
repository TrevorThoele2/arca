#pragma once

#include "Relic.h"

namespace Arca
{
    class Reliquary;

    class OwnedRelic
    {
    protected:
        explicit OwnedRelic(Reliquary& owner);
        explicit OwnedRelic(const ::Inscription::BinaryTableData<OwnedRelic>& data);
        OwnedRelic(const OwnedRelic& arg) = default;

        OwnedRelic& operator=(const OwnedRelic& arg) = default;
    protected:
        [[nodiscard]] Reliquary& Owner();
        [[nodiscard]] const Reliquary& Owner() const;
    private:
        Reliquary* owner;
    private:
        INSCRIPTION_TABLE_ACCESS;
    };
}

namespace Inscription
{
    template<>
    struct TableData<::Arca::OwnedRelic, BinaryArchive> :
        RelicTableDataBase<::Arca::OwnedRelic, BinaryArchive>
    {};

    template<>
    class Scribe<::Arca::OwnedRelic, BinaryArchive> final :
        public RelicScribe<::Arca::OwnedRelic, BinaryArchive>
    {
    public:
        class Table final : public TableBase
        {};
    };
}