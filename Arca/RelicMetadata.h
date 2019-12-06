#pragma once

#include <optional>
#include <vector>

#include "RelicID.h"
#include "RelicOpenness.h"
#include "TypeHandle.h"
#include "HandleSlim.h"

#include "Serialization.h"

namespace Arca
{
    struct RelicMetadata
    {
        RelicID id = 0;
        RelicOpenness openness = RelicOpenness::Open;
        TypeHandle typeHandle;
        void* storage = nullptr;

        std::optional<HandleSlim> parent;
        std::vector<HandleSlim> children;

        RelicMetadata() = default;
        RelicMetadata(
            RelicID id,
            RelicOpenness openness,
            TypeHandle typeHandle,
            void* storage = nullptr);
    };
}