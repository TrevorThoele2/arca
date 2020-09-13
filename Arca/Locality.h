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
    template<class Archive>
    struct ScribeTraits<Arca::Locality, Archive>
    {
        using Category = EnumScribeCategory<Arca::Locality>;
    };
}