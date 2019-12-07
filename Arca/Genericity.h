#pragma once

#include <Inscription/EnumScribe.h>

namespace Arca
{
    enum class Genericity
    {
        Generic,
        Typed
    };
}

namespace Inscription
{
    template<>
    class Scribe<::Arca::Genericity, BinaryArchive> final :
        public EnumScribe<::Arca::Genericity, BinaryArchive>
    {};
}