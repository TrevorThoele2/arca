#pragma once

#include <Chroma/VariadicTemplate.h>

namespace Arca
{
    template<class... Types>
    using RelicTypeList = ::Chroma::VariadicTemplate<Types...>;
}