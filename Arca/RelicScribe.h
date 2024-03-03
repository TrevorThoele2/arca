#pragma once

#include "RelicTraits.h"
#include "Serialization.h"

namespace Inscription
{
    template<class T, class Archive>
    class CompositeRelicScribe : public CompositeScribe<T, Archive>
    {
    private:
        using BaseT = CompositeScribe<T, Archive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    public:
        using BaseT::Scriven;
    public:
        static TypeHandle OutputTypeHandle(const ArchiveT& archive);
    };

    template<class T, class Archive>
    TypeHandle CompositeRelicScribe<T, Archive>::OutputTypeHandle(const ArchiveT& archive)
    {
        return ::Arca::TypeHandleFor<T>().name;
    }

    template<class T, class Archive>
    class TableRelicScribe : public TableScribe<T, Archive>
    {
    private:
        using BaseT = TableScribe<T, Archive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    public:
        using BaseT::Scriven;

        using BaseT::TableBase;
    public:
        static TypeHandle OutputTypeHandle(const ArchiveT& archive);
    };

    template<class T, class Archive>
    TypeHandle TableRelicScribe<T, Archive>::OutputTypeHandle(const ArchiveT& archive)
    {
        return ::Arca::TypeHandleFor<T>().name;
    }
}