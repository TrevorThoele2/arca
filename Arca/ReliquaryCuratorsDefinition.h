#pragma once

#include "ReliquaryCurators.h"
#include "Reliquary.h"

namespace Arca
{
    template<class CuratorT>
    bool ReliquaryCurators::Has() const
    {
        return Owner().Find<CuratorT>() != nullptr;
    }

    template<class Function>
    void ReliquaryCurators::Work(Function function)
    {
        for (auto& stage : workPipeline)
            for (auto& curator : stage)
                function(*curator);
    }
}