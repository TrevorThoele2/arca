#pragma once

#include <optional>
#include <vector>

#include "RelicID.h"
#include "Locality.h"
#include "Type.h"
#include "Handle.h"

namespace Arca
{
    struct RelicMetadata
    {
        RelicID id = nullRelicID;
        Locality locality = Locality::Local;
        Type type;
        void* storage = nullptr;

        std::optional<Handle> parent;
        std::vector<Handle> children;

        bool shouldSerializeBinary = true;
        bool shouldSerializeJson = true;

        RelicMetadata() = default;
        RelicMetadata(
            RelicID id,
            Locality locality,
            Type type,
            void* storage,
            bool shouldSerializeBinary,
            bool shouldSerializeJson);
    };
}