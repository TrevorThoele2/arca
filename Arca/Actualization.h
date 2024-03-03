#pragma once

#include "Handle.h"
#include "Reliquary.h"

namespace Arca
{
    template<class T, std::enable_if_t<!is_either_v<T> && !is_global_relic_v<T>, int> = 0>
    T* Actualize(const Handle& handle)
    {
        if (handle.Type() != TypeFor<T>())
            return {};

        return handle.Owner().Find<T>(handle.ID());
    }

    template<class T, std::enable_if_t<is_global_relic_v<T>, int> = 0>
    T* Actualize(const Handle& handle)
    {
        if (handle.Type() != TypeFor<T>())
            return {};

        return handle.Owner().Find<T>();
    }

    template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int> = 0>
    typename EitherT::ShardT* Actualize(const Handle& handle)
    {
        using BareT = typename EitherT::BareT;
        if (TypeFor<BareT>().name != handle.Type().name)
            return {};

        return handle.Owner().Find<EitherT>(handle.ID());
    }
}