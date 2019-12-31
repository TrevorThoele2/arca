#pragma once

#include <Chroma/VariadicTemplate.h>

namespace Arca
{
    template<class... Ts>
    struct All
    {
        using Pack = ::Chroma::VariadicTemplate<Ts...>;
    };
}