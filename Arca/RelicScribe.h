#pragma once

#include <tuple>

#include "ProcessedRelicTraits.h"

#include "Serialization.h"

#include <Chroma/Iterate.h>

namespace Inscription
{
    template<class T, class Archive>
    class RelicTableDataBase : public TableDataBase<T, Archive>
    {
    private:
        using BaseT = TableDataBase<T, Archive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;

        template<class U>
        using Base = typename BaseT::template Base<U>;
    private:
        using RelicTraits = ::Arca::ProcessedRelicTraits<ObjectT>;
    private:
        template<class BaseT>
        struct BaseTransformer
        {
            using Type = BaseTableDataLink<BaseT, ObjectT, ArchiveT>;
        };
    public:
        using Bases = typename RelicTraits::Bases::template Transform<BaseTransformer>::Type::TupleT;
        Bases bases = {};
    };

    template<class T, class Archive>
    class RelicTableBase : public TableBase<T, Archive>
    {
    private:
        using BaseT = TableBase<T, Archive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
        using DataT = typename BaseT::DataT;
        using BaseT::data;
    public:
        RelicTableBase();
    protected:
        using DataLink = typename BaseT::DataLink;
    private:
        using Bases = typename DataT::Bases;
        constexpr static size_t baseCount = std::tuple_size<Bases>::value;
        constexpr static bool hasBases = baseCount > 0;

        template<size_t i>
        struct BaseIterator
        {
            static void Do(Bases& baseList, RelicTableBase& table)
            {
                table.AddDataLink(DataLink::Base(std::get<i>(baseList)));
            }
        };

        void AddBaseLinks(std::true_type)
        {
            ::Chroma::IterateRange<size_t, BaseIterator, baseCount - 1>(data.bases, *this);
        }

        void AddBaseLinks(std::false_type)
        {}
    };

    template<class T, class Archive>
    RelicTableBase<T, Archive>::RelicTableBase()
    {
        AddBaseLinks(std::bool_constant<hasBases>{});
    }

    template<class T, class Archive>
    class RelicScribe : public TableScribe<T, Archive>
    {
    private:
        using BaseT = TableScribe<T, Archive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    public:
        using DataBase = RelicTableDataBase<ObjectT, ArchiveT>;
        using TableBase = RelicTableBase<ObjectT, ArchiveT>;
    public:
        using BaseT::Scriven;
        using BaseT::Construct;
    public:
        static TypeHandle PrincipleTypeHandle(const ArchiveT& archive);
    };

    template<class T, class Archive>
    TypeHandle RelicScribe<T, Archive>::PrincipleTypeHandle(const ArchiveT& archive)
    {
        return ::Arca::RelicTraits<T>::typeHandle;
    }
}