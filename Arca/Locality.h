#pragma once

#include <Inscription/EnumScribeCategory.h>

namespace Arca
{
    enum class Locality
    {
        Local,
        Global
    };
}

namespace Inscription
{
    template<class Format>
    struct ScribeTraits<Arca::Locality, Format>
    {
        using Category = EnumScribeCategory<Arca::Locality>;
    };
}