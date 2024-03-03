#include "ReliquarySignals.h"

namespace Arca
{
    ReliquarySignals::ReliquarySignals(Reliquary& owner) : ReliquaryComponent(owner, "signal")
    {}

    ReliquarySignals::ListenerListBase::~ListenerListBase() = default;
}