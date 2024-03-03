#pragma once

#include "StaticAssert.h"

#include "Serialization.h"

namespace Inscription
{
    template<class T, class Archive>
    class CuratorScribe : public CompositeScribe<T, Archive>
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
    protected:
        using BaseT::ScrivenImplementation;
    private:
        STATIC_ASSERT_TYPE_DERIVED_FROM_CURATOR(T);
    };

    template<class T, class Archive>
    TypeHandle CuratorScribe<T, Archive>::OutputTypeHandle(const ArchiveT& archive)
    {
        return ::Arca::TypeHandleFor<T>();
    }
}