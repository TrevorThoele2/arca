#pragma once

#include "RelicID.h"
#include "Reliquary.h"

#include "Serialization.h"

namespace Arca
{
    class TypedRelic
    {
    public:
        [[nodiscard]] Reliquary& Owner();
        [[nodiscard]] const Reliquary& Owner() const;
        [[nodiscard]] RelicID ID() const;
    public:
        static const RelicStructure structure;
    protected:
        TypedRelic(RelicID id, Reliquary& owner);
        explicit TypedRelic(const ::Inscription::BinaryTableData<TypedRelic>& data);
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
    private:
        friend Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };
}

namespace Inscription
{
    template<>
    struct TableData<::Arca::TypedRelic, BinaryArchive> final :
        TableDataBase<::Arca::TypedRelic, BinaryArchive>
    {
        ::Arca::RelicID id;
        ::Arca::Reliquary* owner;
    };

    template<>
    class Scribe<::Arca::TypedRelic, BinaryArchive> final :
        public TableScribe<::Arca::TypedRelic, BinaryArchive>
    {
    public:
        class Table : public TableBase
        {
        public:
            Table()
            {
                MergeDataLinks({
                    DataLink::Auto(&ObjectT::id, &DataT::id),
                    DataLink::Auto(&ObjectT::owner, &DataT::owner) }
                );
            }
        };
    };
}