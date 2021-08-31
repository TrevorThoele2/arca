#include "ReliquarySignals.h"

namespace Arca
{
    ReliquarySignals::ReliquarySignals(Reliquary& owner) : owner(&owner)
    {}

    ReliquarySignals::ListenerListBase::~ListenerListBase() = default;
}