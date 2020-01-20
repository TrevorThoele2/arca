#pragma once

#include <Inscription/NullScribe.h>
#include <Inscription/Scribe.h>

namespace Arca
{
    template<class T>
    constexpr static bool HasScribe()
    {
        return !std::is_base_of_v<
            ::Inscription::NullScribe<std::remove_const_t<T>, ::Inscription::BinaryArchive>,
            ::Inscription::Scribe<std::remove_const_t<T>, ::Inscription::BinaryArchive>>;
    }
}