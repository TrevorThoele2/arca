#include "ReliquaryRelicStructures.h"

namespace Arca
{
    ReliquaryRelicStructures::ReliquaryRelicStructures(Reliquary& owner)
        : ReliquaryComponent(owner, "relic structure")
    {}

    ReliquaryRelicStructures::Named::Named(std::string name, RelicStructure value) :
        name(std::move(name)), value(std::move(value))
    {}
}