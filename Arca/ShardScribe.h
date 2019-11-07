#pragma once

#include "ShardTraits.h"
#include "Serialization.h"

namespace Inscription
{
    template<class T, class Archive>
    class ShardScribe : public CompositeScribe<T, Archive>
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
    TypeHandle ShardScribe<T, Archive>::OutputTypeHandle(const ArchiveT& archive)
    {
        return ::Arca::ShardTraits<T>::typeHandle;
    }
}