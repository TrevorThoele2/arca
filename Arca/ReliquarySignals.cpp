#include "ReliquarySignals.h"

namespace Arca
{
    ReliquarySignals::BatchSources::BatchSources(ReliquarySignals& owner) : BatchSourcesBase(owner)
    {}

    ReliquarySignals::ReliquarySignals(Reliquary& owner) : ReliquaryComponent(owner, "signal")
    {}
}