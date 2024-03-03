#pragma once

#include <Inscription/CompositeScribe.h>

#include "Traits.h"

namespace Inscription
{
    template<class T, class Archive>
    class ArcaCompositeScribe : public CompositeScribe<T, Archive>
    {
    private:
        using BaseT = CompositeScribe<T, Archive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    public:
        using BaseT::Scriven;
    public:
        static Type OutputType(const ArchiveT& archive);
    };

    template<class T, class Archive>
    Type ArcaCompositeScribe<T, Archive>::OutputType(const ArchiveT& archive)
    {
        return ::Arca::TypeFor<T>().name;
    }
}