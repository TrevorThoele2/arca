#include "SignalBatchException.h"

namespace Arca
{
    SignalBatchInvalidated::SignalBatchInvalidated() : runtime_error("This batch has been invalidated")
    {}
}