#pragma once

#include <Inscription/EnumScribe.h>

namespace Arca
{
    enum class VesselDynamism
    {
        Fixed,
        Dynamic,
        Static
    };
}

namespace Inscription
{
    template<>
    class Scribe<::Arca::VesselDynamism, BinaryArchive> final :
        public EnumScribe<::Arca::VesselDynamism, BinaryArchive>
    {};
}