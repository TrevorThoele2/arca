#pragma once

#include <Inscription/CompositeScribeCategory.h>

#include "TypeFor.h"

#include "Traits.h"

namespace Inscription
{
    template<class T>
    class ArcaCompositeScribeCategory
    {
    public:
        using ObjectT = T;
    public:
        static constexpr bool requiresScribe = true;
        using ScribeT = Scribe<ObjectT>;
    public:
        static void Scriven(ObjectT& object, Format::Binary& format, ScribeT& scribe);
        static void Scriven(const std::string& name, ObjectT& object, Format::Json& format, ScribeT& scribe);
    public:
        template<class Format>
        static Type OutputType(const Format& format);
    };

    template<class T>
    void ArcaCompositeScribeCategory<T>::Scriven(ObjectT& object, Format::Binary& format, ScribeT& scribe)
    {
        CompositeScribeCategory<T>::Scriven(object, format, scribe);
    }

    template<class T>
    void ArcaCompositeScribeCategory<T>::Scriven(const std::string& name, ObjectT& object, Format::Json& format, ScribeT& scribe)
    {
        CompositeScribeCategory<T>::Scriven(name, object, format, scribe);
    }

    template<class T>
    template<class Format>
    Type ArcaCompositeScribeCategory<T>::OutputType(const Format&)
    {
        return Arca::TypeFor<T>().name;
    }
}