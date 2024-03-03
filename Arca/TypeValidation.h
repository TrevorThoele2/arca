#pragma once

#include <type_traits>

namespace Arca
{
    class Curator;

    template<class T>
    struct IsCurator : std::is_base_of<Curator, T>
    {};
}