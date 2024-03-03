#include "SignalBatchSource.h"

namespace Arca
{
    SignalBatchSourceBase::~SignalBatchSourceBase() = default;

    SignalBatchSourceBase::SignalBatchSourceBase(Reliquary& owner) : owner(&owner)
    {}
}