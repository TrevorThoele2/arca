#include "ReliquaryRelicStructures.h"

#include "ReliquaryException.h"

namespace Arca
{
    RelicStructure ReliquaryRelicStructures::RequiredRelicStructure(const std::string & name) const
    {
        for (auto& loop : namedList)
            if (loop.name == name)
                return loop.value;

        throw NotRegistered(objectTypeName, Type(name));
    }

    ReliquaryRelicStructures::Named::Named(std::string name, RelicStructure value) :
        name(std::move(name)), value(std::move(value))
    {}
}