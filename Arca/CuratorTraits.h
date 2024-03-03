#pragma once

#include "CuratorTypeDescription.h"

namespace Arca
{
    template<class T>
    struct CuratorTraits;

    template<class T>
    CuratorTypeDescription CuratorTypeDescriptionFor()
    {
        return CuratorTypeDescription(CuratorTraits<T>::typeHandle, [](::Inscription::BinaryArchive& archive)
            {
                return ::Inscription::InputTypeHandlesFor<T>(archive);
            });
    }
}
