#include "ReliquarySignals.h"

namespace Arca
{
    ReliquarySignals::BatchSources::BatchSources(ReliquarySignals& owner) : StorageBatchSources(owner)
    {}

    ReliquarySignals::ReliquarySignals(Reliquary& owner) : ReliquaryComponent(owner, "signal")
    {}
}