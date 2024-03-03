#pragma once

#include "RelicID.h"
#include "Handle.h"
#include "RelicStructure.h"

namespace Arca
{
    struct CreateData
    {
        std::optional<RelicID> id = {};
        std::optional<Handle> parent = {};
        std::optional<std::variant<RelicStructure, std::string>> structure = {};
    };
}