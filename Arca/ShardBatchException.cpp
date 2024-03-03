#include "ShardBatchException.h"

namespace Arca
{
    ShardBatchNotSetup::ShardBatchNotSetup() : runtime_error("This batch has not been setup. It was probably default constructed.")
    {}
}