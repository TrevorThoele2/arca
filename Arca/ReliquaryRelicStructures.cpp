#include "ReliquaryRelicStructures.h"

namespace Arca
{
    ReliquaryRelicStructures::ReliquaryRelicStructures(Reliquary& owner)
        : ReliquaryComponent(owner, "relic structure")
    {}

    RelicStructure ReliquaryRelicStructures::RequiredRelicStructure(const std::string & name) const
    {
        for (auto& loop : namedList)
            if (loop.name == name)
                return loop.value;

        throw NotRegistered(name);
    }

    ReliquaryRelicStructures::Named::Named(std::string name, RelicStructure value) :
        name(std::move(name)), value(std::move(value))
    {}
}