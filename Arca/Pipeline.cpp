#include "Pipeline.h"

namespace Arca
{
    std::vector<TypeName> Stage::TypeNameList() const
    {
        return typeNames;
    }

    bool Stage::Empty() const
    {
        return typeNames.empty();
    }
}