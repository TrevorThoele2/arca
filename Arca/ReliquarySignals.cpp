#include "ReliquarySignals.h"

namespace Arca
{
    ReliquarySignals::ReliquarySignals(Reliquary& owner) : ReliquaryComponent(owner, "signal")
    {}

    SignalBatchSourceBase* ReliquarySignals::FindBatchSource(const std::type_index& type)
    {
        const auto found = batchSources.find(type);
        if (found == batchSources.end())
            return nullptr;

        return found->second.get();
    }
}