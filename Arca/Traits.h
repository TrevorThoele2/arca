#pragma once

#include <type_traits>

#include "ObjectType.h"
#include "Type.h"

#include <Inscription/NullScribe.h>

namespace Arca
{
    template<class T>
    struct Traits;

    template<class T>
    ObjectType ObjectTypeFor()
    {
        return Traits<T>::objectType;
    }

    template<class T>
    constexpr static bool HasScribe()
    {
        return !std::is_base_of_v<
            ::Inscription::NullScribe<T, ::Inscription::BinaryArchive>,
            ::Inscription::Scribe<T, ::Inscription::BinaryArchive>>;
    }
}