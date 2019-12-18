#pragma once

#include <type_traits>

#include "Type.h"
#include "ObjectType.h"

#include <Inscription/NullScribe.h>

namespace Arca
{
    template<class T>
    struct Traits;

    template<class T, std::enable_if_t<!std::is_const_v<T>, int> = 0>
    Type TypeFor()
    {
        return { Traits<std::decay_t<T>>::typeName, false };
    }

    template<class T, std::enable_if_t<std::is_const_v<T>, int> = 0>
    Type TypeFor()
    {
        return { Traits<std::decay_t<T>>::typeName, true };
    }

    template<class T>
    constexpr static bool HasScribe()
    {
        return !std::is_base_of_v<
            ::Inscription::NullScribe<T, ::Inscription::BinaryArchive>,
            ::Inscription::Scribe<T, ::Inscription::BinaryArchive>>;
    }
}