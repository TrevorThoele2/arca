#include "ReliquaryCurators.h"

namespace Arca
{
    ReliquaryCurators::ReliquaryCurators(Reliquary& owner) : ReliquaryComponent(owner, "curator")
    {}

    void ReliquaryCurators::DoOn(void(*function)(Curator& curator))
    {
        for (auto& stage : workPipeline)
            for (auto& curator : stage)
                function(*curator);
    }

    void ReliquaryCurators::DoOn(void(*function)(Curator& curator, Curator::Stage& stage))
    {
        Curator::Stage stageHandle;

        for (auto& stage : workPipeline)
        {
            for (auto& curator : stage)
            {
                function(*curator, stageHandle);
                if (stageHandle.IsAborted())
                    return;
            }
        }
    }
}