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
        static void Scriven(ObjectT& object, Format::Binary& format);
        static void Scriven(const std::string& name, ObjectT& object, Format::Json& format);
    public:
        template<class Format>
        static Type OutputType(const Format& format);
    };

    template<class T>
    void ArcaNullScribeCategory<T>::Scriven(ObjectT& object, Format::Binary& format)
    {
        NullScribeCategory<T>::Scriven(object, format);
    }

    template<class T>
    void ArcaNullScribeCategory<T>::Scriven(const std::string& name, ObjectT& object, Format::Json& format)
    {
        NullScribeCategory<T>::Scriven(name, object, format);
    }

    template<class T>
    template<class Format>
    Type ArcaNullScribeCategory<T>::OutputType(const Format&)
    {
        return Arca::TypeFor<T>().name;
    }
}