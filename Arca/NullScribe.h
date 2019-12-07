#pragma once

#include <Inscription/NullScribe.h>

#include "Traits.h"

namespace Inscription
{
    template<class T, class Archive>
    class ArcaNullScribe : public NullScribe<T, Archive>
    {
    private:
        using BaseT = NullScribe<T, Archive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    public:
        using BaseT::Scriven;
    public:
        static Type OutputType(const ArchiveT& archive);
    };

    template<class T, class Archive>
    Type ArcaNullScribe<T, Archive>::OutputType(const ArchiveT& archive)
    {
        return ::Arca::TypeFor<T>().name;
    }
}