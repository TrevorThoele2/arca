#pragma once

#include <Inscription/EnumScribe.h>

namespace Arca
{
    enum class RelicOpenness
    {
        Open,
        Fixed,
        Typed,
        Global
    };
}

namespace Inscription
{
    template<>
    class Scribe<::Arca::RelicOpenness, BinaryArchive> final :
        public EnumScribe<::Arca::RelicOpenness, BinaryArchive>
    {};
}