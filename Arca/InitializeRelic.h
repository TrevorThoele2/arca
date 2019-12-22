#pragma once

#include "HasInitializeMethod.h"

namespace Arca
{
    template<class T, class... InitializeArgs, std::enable_if_t<has_initialize_method_v<T>, int> = 0>
    void InitializeRelic(T& relic, InitializeArgs&& ... initializeArgs)
    {
        relic.Initialize(std::forward<InitializeArgs>(initializeArgs)...);
    }

    template<class T, class... InitializeArgs, std::enable_if_t<!has_initialize_method_v<T>, int> = 0>
    void InitializeRelic(T& relic, InitializeArgs&& ... initializeArgs)
    {}
}