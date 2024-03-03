#pragma once

#include <optional>
#include <vector>

#include "RelicID.h"
#include "Openness.h"
#include "Locality.h"
#include "TypeHandle.h"
#include "HandleSlim.h"

namespace Arca
{
    struct RelicMetadata
    {
        RelicID id = 0;
        Openness openness = Openness::Open;
        Locality locality = Locality::Local;
        TypeHandle typeHandle;
        void* storage = nullptr;

        std::optional<HandleSlim> parent;
        std::vector<HandleSlim> children;

        RelicMetadata() = default;
        RelicMetadata(
            RelicID id,
            Openness openness,
            Locality locality,
            TypeHandle typeHandle,
            void* storage = nullptr);
    };
}