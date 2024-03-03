#pragma once

#include "Handle.h"
#include "LocalPtr.h"
#include "GlobalPtr.h"

namespace Arca
{
    template<class T, std::enable_if_t<!(is_global_v<T> && is_relic_v<T>), int> = 0>
    LocalPtr<T> Actualize(const Handle& handle)
    {
        if (handle.Type() != TypeFor<T>())
            return {};

        return LocalPtr<T>(handle.ID(), handle.Owner());
    }

    template<class GlobalRelicT, std::enable_if_t<is_global_v<GlobalRelicT> && is_relic_v<GlobalRelicT>, int> = 0>
    GlobalPtr<GlobalRelicT> Actualize(const Handle& handle)
    {
        if (handle.Type() != TypeFor<GlobalRelicT>())
            return {};

        return GlobalPtr<GlobalRelicT>(handle.Owner());
    }
}