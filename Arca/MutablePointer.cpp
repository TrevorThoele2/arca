#include "MutablePointer.h"

namespace Arca
{
    Arca::Reliquary& MutablePointer::Reliquary()
    {
        return *reliquary;
    }

    const Arca::Reliquary& MutablePointer::Reliquary() const
    {
        return *reliquary;
    }

    MutablePointer::MutablePointer(Arca::Reliquary& reliquary) : reliquary(&reliquary)
    {}
}