#include "ReliquaryMatrices.h"

namespace Arca
{
    void ReliquaryMatrices::Clear()
    {
        knownList.clear();
        batchSources.Clear();
    }

    void ReliquaryMatrices::NotifyCreated(RelicID id)
    {
        for (auto& loop : knownList)
            loop.value.Created(id, Owner());
    }

    void ReliquaryMatrices::NotifyDestroying(RelicID id)
    {
        for (auto& loop : knownList)
            loop.value.Destroying(id, Owner());
    }

    std::vector<KnownMatrix> ReliquaryMatrices::AllKnown() const
    {
        std::vector<KnownMatrix> returnValue;
        for (auto& loop : knownList)
            returnValue.push_back(loop.value);
        return returnValue;
    }

    ReliquaryMatrices::BatchSources::BatchSources(ReliquaryMatrices& owner) : owner(&owner)
    {}

    ReliquaryMatrices::ReliquaryMatrices(Reliquary& owner) : ReliquaryComponent(owner, "matrix")
    {}
}