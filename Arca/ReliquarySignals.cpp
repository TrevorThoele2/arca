#include "ReliquarySignals.h"

namespace Arca
{
    ReliquarySignals::ReliquarySignals(ReliquaryMatrices& matrices) : matrices(&matrices)
    {}

    ReliquarySignals::ListenerListBase::~ListenerListBase() = default;
}