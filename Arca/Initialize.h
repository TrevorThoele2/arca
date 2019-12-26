#pragma once

#include "HasInitializeMethod.h"

namespace Arca
{
    template<class T, class... InitializeArgs, std::enable_if_t<has_initialize_method_v<T>, int> = 0>
    void Initialize(T& object, InitializeArgs&& ... initializeArgs)
    {
        object.Initialize(std::forward<InitializeArgs>(initializeArgs)...);
    }

    template<class T, class... InitializeArgs, std::enable_if_t<!has_initialize_method_v<T>, int> = 0>
    void Initialize(T&, InitializeArgs&& ...)
    {}
}