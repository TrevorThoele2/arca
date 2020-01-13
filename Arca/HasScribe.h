#pragma once

#include <Inscription/NullScribe.h>

namespace Arca
{
    template<class T>
    constexpr static bool HasScribe()
    {
        return !std::is_base_of_v<
            ::Inscription::NullScribe<T, ::Inscription::BinaryArchive>,
            ::Inscription::Scribe<T, ::Inscription::BinaryArchive>>;
    }
}