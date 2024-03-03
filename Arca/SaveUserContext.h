#pragma once

#include <set>
#include "RelicID.h"

namespace Inscription
{
    struct SaveUserContext
    {
        std::set<Arca::RelicID> ids;
    };
}