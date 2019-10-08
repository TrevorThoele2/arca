#include "Exception.h"

namespace Arca
{
    Exception::Exception(const std::string& message) : runtime_error(message)
    {}
}