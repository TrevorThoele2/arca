#include "CuratorPipeline.h"

namespace Arca
{
    std::vector<TypeHandle> CuratorStage::TypeHandleList() const
    {
        return typeHandles;
    }
}