#pragma once

#include <string>
#include <vector>

#include "RelicStructure.h"

namespace Arca
{
    class ReliquaryRelicStructures
    {
    public:
        ReliquaryRelicStructures() = default;

        [[nodiscard]] RelicStructure RequiredRelicStructure(const std::string& name) const;
    public:
        struct Named
        {
            std::string name;
            RelicStructure value;

            Named(std::string name, RelicStructure value);
        };
        using NamedList = std::vector<Named>;
        NamedList namedList{};
    private:
        const std::string objectTypeName = "relic structure";
    };
}
