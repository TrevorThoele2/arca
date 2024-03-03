#pragma once

#include "HasBareInitializeMethod.h"

namespace Arca
{
    template<class T, class... InitializeArgs>
    void Initialize(T& object, InitializeArgs&& ... initializeArgs)
    {
        object.Initialize(std::forward<InitializeArgs>(initializeArgs)...);
    }

    template<class T, std::enable_if_t<has_bare_initialize_method_v<T>, int> = 0>
    void Initialize(T& object)
    {
        object.Initialize();
    }

    template<class T, std::enable_if_t<!has_bare_initialize_method_v<T>, int> = 0>
    void Initialize(T&)
    {}
}