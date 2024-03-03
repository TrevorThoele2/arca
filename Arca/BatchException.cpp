#include "BatchException.h"

namespace Arca
{
    BatchNotSetup::BatchNotSetup() : runtime_error("This batch has not been setup. It was probably default constructed.")
    {}
}