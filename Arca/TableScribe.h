#pragma once

#include <Inscription/TableScribe.h>

#include "Traits.h"

namespace Inscription
{
    template<class T, class Archive>
    class ArcaTableScribe : public TableScribe<T, Archive>
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
    TypeHandle ArcaTableScribe<T, Archive>::OutputTypeHandle(const ArchiveT& archive)
    {
        return ::Arca::TypeHandleFor<T>().name;
    }
}