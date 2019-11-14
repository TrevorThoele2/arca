#pragma once

#include "RelicID.h"
#include "RelicStructure.h"
#include "StructureFrom.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    class TypedRelic
    {
    public:
        [[nodiscard]] RelicID ID() const;
    public:
        virtual ~TypedRelic() = 0;

        virtual void Initialize(Reliquary& reliquary) = 0;
        [[nodiscard]] virtual RelicStructure Structure() const = 0;
    protected:
        TypedRelic() = default;
        explicit TypedRelic(const ::Inscription::BinaryTableData<TypedRelic>& data);
    private:
        RelicID id = 0;
    private:
        friend class Reliquary;
        friend class ReliquaryOrigin;
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
                    DataLink::Auto(&ObjectT::id, &DataT::id) }
                );
            }
        };
    };
}