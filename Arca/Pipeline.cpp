#include "Pipeline.h"

namespace Arca
{
    std::vector<TypeHandle> Stage::TypeHandleList() const
    {
        return typeHandles;
    }

    bool Stage::Empty() const
    {
        return typeHandles.empty();
    }
}