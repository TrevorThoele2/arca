#pragma once

#include <Inscription/TableScribe.h>
#include <Inscription/BinaryArchive.h>

#include "Traits.h"

namespace Inscription
{
    template<class T, class Archive>
    class ArcaTableScribe;

    template<class T>
    class ArcaTableScribe<T, BinaryArchive> : public TableScribe<T, BinaryArchive>
    {
    private:
        using BaseT = TableScribe<T, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    public:
        using BaseT::Scriven;

        using BaseT::TableBase;
    public:
        static Type OutputType(const ArchiveT& archive);
    };

    template<class T>
    Type ArcaTableScribe<T, BinaryArchive>::OutputType(const ArchiveT& archive)
    {
        return ::Arca::TypeFor<T>().name;
    }
}