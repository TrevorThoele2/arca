#pragma once

#include <Chroma/VariadicTemplate.h>

namespace Arca
{
    template<class... Args>
    using CuratorDependencyList = ::Chroma::VariadicTemplate<Args...>;
}