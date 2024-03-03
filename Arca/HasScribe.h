#pragma once

#include "NullScribeCategory.h"

namespace Arca
{
    template<class T, class Format>
    constexpr static bool HasScribe()
    {
        using ObjectT = Inscription::remove_const_t<T>;
        using IsNullInscription = std::is_same<
            Inscription::NullScribeCategory<ObjectT>,
            typename Inscription::ScribeTraits<ObjectT, Format>::Category>;
        using IsNullArca = std::is_same<
            Inscription::ArcaNullScribeCategory<ObjectT>,
            typename Inscription::ScribeTraits<ObjectT, Format>::Category>;
        return std::negation<std::disjunction<IsNullInscription, IsNullArca>>::value;
    }
}