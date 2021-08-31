#include "ReliquaryCommands.h"

#include "ReliquaryRelics.h"

#include "ReliquaryException.h"

namespace Arca
{
    CommandHandlerBase& ReliquaryCommands::RequiredHandler(const TypeName& typeName)
    {
        const auto found = handlerMap.find(typeName);
        if (found == handlerMap.end())
            throw NotRegistered(objectTypeName, Type{ typeName });

        return *found->second;
    }

    ReliquaryCommands::ReliquaryCommands(Reliquary& owner, ReliquaryCurators& curators) : owner(&owner), curators(&curators)
    {}
}