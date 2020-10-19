#include "Clear.h"

#include "Reliquary.h"

namespace Arca
{
    Clear::Clear(Type type) : type(std::move(type))
    {}

    void Clear::Do(Reliquary& reliquary) const
    {
        reliquary.relics.Clear(type.name);
    }
}