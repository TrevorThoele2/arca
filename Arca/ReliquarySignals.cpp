#include "ReliquarySignals.h"

namespace Arca
{
    ReliquarySignals::BatchSources::BatchSources(ReliquarySignals& owner) :
        owner(&owner)
    {}

    ReliquarySignals::ReliquarySignals(Reliquary& owner) : ReliquaryComponent(owner, "signal")
    {}
}