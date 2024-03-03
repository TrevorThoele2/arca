#pragma once

#include "ReliquaryComponent.h"

#include <string>
#include <vector>

#include "RelicStructure.h"

namespace Arca
{
    class ReliquaryRelicStructures : public ReliquaryComponent
    {
    public:
        explicit ReliquaryRelicStructures(Reliquary& owner);
    public:
        struct Named
        {
            std::string name;
            RelicStructure value;

            Named(std::string name, RelicStructure value);
        };
        using NamedList = std::vector<Named>;
        NamedList namedList;
    };
}
