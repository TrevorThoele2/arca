#include "Pipeline.h"

namespace Arca
{
    std::vector<TypeHandleName> Stage::TypeHandleNameList() const
    {
        return typeHandleNames;
    }

    bool Stage::Empty() const
    {
        return typeHandleNames.empty();
    }
}