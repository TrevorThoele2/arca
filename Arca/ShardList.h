#pragma once

#include <Chroma/VariadicTemplate.h>

namespace Arca
{
    template<class... Args>
    using ShardList = Chroma::VariadicTemplate<Args...>;
}