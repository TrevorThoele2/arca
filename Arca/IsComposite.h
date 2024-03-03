#pragma once

#include "IsAll.h"
#include "UsableInAll.h"

namespace Arca
{
    template<class ShardsT, class Enable = void>
    struct is_composite : std::false_type
    {};

    template<class ShardsT>
    struct is_composite<ShardsT, std::enable_if_t<
        is_all_v<ShardsT>
        && (ShardsT::Pack::count > 1)
        && ShardsT::Pack::template ForwardArguments<usable_in_all>::Type::value>>
        : std::true_type
    {};

    template<class ShardsT>
    static constexpr bool is_composite_v = is_composite<ShardsT>::value;
}