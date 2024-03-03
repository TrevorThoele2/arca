#include "ReliquaryCurators.h"

namespace Arca
{
    Curator& ReliquaryCurators::Find(const TypeName& type)
    {
        const auto found = FindHandler(type);
        if (found == nullptr)
            throw this->NotRegistered(Type{ type });

        return found->Value();
    }

    const Curator& ReliquaryCurators::Find(const TypeName& type) const
    {
        return const_cast<ReliquaryCurators&>(*this).Find(type);
    }

    ReliquaryCurators::HandlerBase::~HandlerBase() = default;

    ReliquaryCurators::HandlerBase::HandlerBase(const TypeName& typeName) : typeName(typeName)
    {}

    ReliquaryCurators::HandlerBase* ReliquaryCurators::FindHandler(const TypeName & typeName) const
    {
        const auto found = std::find_if(
            handlers.begin(),
            handlers.end(),
            [typeName](const HandlerPtr& entry)
            {
                return entry->typeName == typeName;
            });
        if (found == handlers.end())
            return nullptr;

        return found->get();
    }

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

    ReliquaryCurators::ReliquaryCurators(Reliquary& owner) : ReliquaryComponent(owner, "curator")
    {}
}