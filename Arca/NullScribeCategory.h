#pragma once

#include <Inscription/NullScribeCategory.h>

#include "TypeFor.h"

#include "Traits.h"

namespace Inscription
{
    template<class T>
    class ArcaNullScribeCategory
    {
    public:
        using ObjectT = T;
    public:
        static constexpr bool requiresScribe = false;
    public:
        static void Scriven(ObjectT& object, Archive::Binary& archive);
        static void Scriven(const std::string& name, ObjectT& object, Archive::Json& archive);
    public:
        template<class Archive>
        static Type OutputType(const Archive& archive);
    };

    template<class T>
    void ArcaNullScribeCategory<T>::Scriven(ObjectT& object, Archive::Binary& archive)
    {
        NullScribeCategory<T>::Scriven(object, archive);
    }

    template<class T>
    void ArcaNullScribeCategory<T>::Scriven(const std::string& name, ObjectT& object, Archive::Json& archive)
    {
        NullScribeCategory<T>::Scriven(name, object, archive);
    }

    template<class T>
    template<class Archive>
    Type ArcaNullScribeCategory<T>::OutputType(const Archive&)
    {
        return Arca::TypeFor<T>().name;
    }
}