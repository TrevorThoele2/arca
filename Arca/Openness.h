#pragma once

#include <Inscription/EnumScribe.h>

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
    template<>
    class Scribe<::Arca::Openness, BinaryArchive> final :
        public EnumScribe<::Arca::Openness, BinaryArchive>
    {};
}