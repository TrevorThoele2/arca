#include "RelicBatchException.h"

namespace Arca
{
    RelicBatchNotSetup::RelicBatchNotSetup() : runtime_error("This batch has not been setup. It was probably default constructed.")
    {}
}