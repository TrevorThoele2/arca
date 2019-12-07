#pragma once

#include <Inscription/EnumScribe.h>

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
    template<>
    class Scribe<::Arca::Locality, BinaryArchive> final :
        public EnumScribe<::Arca::Locality, BinaryArchive>
    {};
}