#include "RelicBatchException.h"

namespace Arca
{
    RelicBatchInvalidated::RelicBatchInvalidated() : runtime_error("This batch has been invalidated")
    {}
}