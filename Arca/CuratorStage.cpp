#include "CuratorStage.h"

namespace Arca
{
    void CuratorStage::Abort()
    {
        isAborted = true;
    }

    bool CuratorStage::IsAborted() const
    {
        return isAborted;
    }
}