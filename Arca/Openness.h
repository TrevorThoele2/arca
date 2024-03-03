#pragma once

#include <Inscription/EnumScribeCategory.h>

namespace Arca
{
    enum class Openness
    {
        Open,
        Closed
    };
}

namespace Inscription
{
    template<class Archive>
    struct ScribeTraits<Arca::Openness, Archive>
    {
        using Category = EnumScribeCategory<Arca::Openness>;
    };
}