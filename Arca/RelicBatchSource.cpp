#include "RelicBatchSource.h"

namespace Arca
{
    RelicBatchSourceBase::~RelicBatchSourceBase() = default;

    RelicBatchSourceBase::RelicBatchSourceBase(Reliquary& owner) : owner(&owner)
    {}
}