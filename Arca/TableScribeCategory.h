#pragma once

#include <Inscription/TableScribeCategory.h>

#include "TypeFor.h"

#include "Traits.h"

namespace Inscription
{
    template<class T>
    class ArcaTableScribeCategory
    {
    public:
        using ObjectT = T;
    public:
        static constexpr bool requiresScribe = true;
        using ScribeT = Scribe<ObjectT>;
    public:
        static void Scriven(ObjectT& object, Archive::Binary& archive, ScribeT& scribe);
        static void Scriven(const std::string& name, ObjectT& object, Archive::Json& archive, ScribeT& scribe);
    public:
        template<class Archive>
        static Type OutputType(const Archive& archive);
    };

    template<class T>
    void ArcaTableScribeCategory<T>::Scriven(ObjectT& object, Archive::Binary& archive, ScribeT& scribe)
    {
        CompositeScribeCategory<T>::Scriven(object, archive, scribe);
    }

    template<class T>
    void ArcaTableScribeCategory<T>::Scriven(const std::string& name, ObjectT& object, Archive::Json& archive, ScribeT& scribe)
    {
        CompositeScribeCategory<T>::Scriven(name, object, archive, scribe);
    }

    template<class T>
    template<class Archive>
    Type ArcaTableScribeCategory<T>::OutputType(const Archive&)
    {
        return Arca::TypeFor<T>().name;
    }
}