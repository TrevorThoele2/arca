#pragma once

#include <Inscription/CompositeScribe.h>
#include <Inscription/BinaryArchive.h>

#include "Traits.h"

namespace Inscription
{
    template<class T, class Archive>
    class ArcaCompositeScribe;

    template<class T>
    class ArcaCompositeScribe<T, BinaryArchive> : public CompositeScribe<T, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<T, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    public:
        using BaseT::Scriven;
    public:
        static Type OutputType(const ArchiveT& archive);
    };

    template<class T>
    Type ArcaCompositeScribe<T, BinaryArchive>::OutputType(const ArchiveT& archive)
    {
        return ::Arca::TypeFor<T>().name;
    }
}