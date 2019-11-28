#pragma once

#include "RelicID.h"
#include "RelicStructure.h"
#include "RelicScribe.h"
#include "RelicTraits.h"
#include "Handle.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;
    class DynamicRelic;

    class TypedRelic
    {
    public:
        operator Handle() const;

        void ParentTo(const Handle& parent) const;
        std::optional<Handle> Parent() const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Reliquary& Owner() const;
    public:
        virtual ~TypedRelic() = 0;

        void Initialize(Reliquary& owner);
    protected:
        TypedRelic() = default;
        explicit TypedRelic(const ::Inscription::BinaryTableData<TypedRelic>& data);

        virtual void InitializeImplementation() {}
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
    private:
        friend class Reliquary;
        friend class ReliquaryOrigin;
        template<class T, class>
        friend class BatchSource;
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