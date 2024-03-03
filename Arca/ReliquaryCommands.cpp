#include "ReliquaryCommands.h"

#include "ReliquaryRelics.h"

namespace Arca
{
    void ReliquaryCommands::Do(const Clear& command)
    {
        Relics().Clear(command.type.name);
    }

    ReliquaryCommands::ReliquaryCommands(Reliquary& owner) : ReliquaryComponent(owner, "command")
    {}
}