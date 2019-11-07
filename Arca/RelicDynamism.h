#pragma once

#include <Inscription/EnumScribe.h>

namespace Arca
{
    enum class RelicDynamism
    {
        Fixed,
        Dynamic,
        Static
    };
}

namespace Inscription
{
    template<>
    class Scribe<::Arca::RelicDynamism, BinaryArchive> final :
        public EnumScribe<::Arca::RelicDynamism, BinaryArchive>
    {};
}