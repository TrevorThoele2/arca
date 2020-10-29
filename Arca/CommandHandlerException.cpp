#include "CommandHandlerException.h"

namespace Arca
{
    CommandNotRegisteredInCurator::CommandNotRegisteredInCurator(
        const TypeName& curatorType, const TypeName& commandType)
        :
        Exception("The command (" + commandType + ") has not been linked in the curator (" + curatorType + ").")
    {}
}