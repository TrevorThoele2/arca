#include "ReliquarySignals.h"

namespace Arca
{
    void ReliquarySignals::Clear()
    {
        for (auto& batchSource : batchSources.map)
            batchSource.second->Clear();

        if (batchSources.transferableSignals)
            batchSources.transferableSignals->Clear();
    }

    ReliquarySignals::BatchSources::BatchSources(ReliquarySignals& owner) :
        owner(&owner)
    {}

    ReliquarySignals::ReliquarySignals(Reliquary& owner) : ReliquaryComponent(owner, "signal")
    {}
}