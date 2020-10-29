#include "ReliquaryCommands.h"

#include "ReliquaryRelics.h"

namespace Arca
{
    CommandHandlerBase& ReliquaryCommands::RequiredHandler(const TypeName& typeName)
    {
        const auto found = handlerMap.find(typeName);
        if (found == handlerMap.end())
            throw NotRegistered(Type{ typeName });

        return *found->second;
    }

    ReliquaryCommands::ReliquaryCommands(Reliquary& owner) : ReliquaryComponent(owner, "command")
    {}
}