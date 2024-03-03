#pragma once

#include <function2/function2.hpp>

namespace Arca
{
    template<class... CallArgs, class Function, class... BoundArgs>
    fu2::unique_function<void(CallArgs...)> CreateOperation(Function function, BoundArgs&& ... boundArgs)
    {
        return fu2::unique_function<void(CallArgs...)>(
            [function, boundArgs = std::make_tuple(std::forward<BoundArgs>(boundArgs) ...)](CallArgs ... callArgs) mutable
        {
            return std::apply(
                [function](auto&& ... args)
                {
                    function(std::forward<decltype(args)>(args)...);
                },
                std::tuple_cat(std::forward_as_tuple(std::forward<CallArgs>(callArgs)...), std::move(boundArgs)));
        });
    }
}