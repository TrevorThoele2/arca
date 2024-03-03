#pragma once

#include <Chroma/VariadicTemplate.h>

namespace Arca
{
    template<class... Args>
    using HandledCommands = Chroma::VariadicTemplate<Args...>;
}