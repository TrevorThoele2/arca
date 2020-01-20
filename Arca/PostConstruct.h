#pragma once

#include "HasPostConstructMethod.h"

namespace Arca
{
    template<class T, std::enable_if_t<has_post_construct_method_v<T>, int> = 0>
    void PostConstruct(T& object)
    {
        object.PostConstruct();
    }

    template<class T, std::enable_if_t<!has_post_construct_method_v<T>, int> = 0>
    void PostConstruct(T&)
    {}
}