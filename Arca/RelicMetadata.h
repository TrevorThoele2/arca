#pragma once

#include <optional>
#include <vector>

#include "RelicID.h"
#include "Openness.h"
#include "Locality.h"
#include "Type.h"
#include "HandleSlim.h"

namespace Arca
{
    struct RelicMetadata
    {
        RelicID id = nullRelicID;
        Openness openness = Openness::Open;
        Locality locality = Locality::Local;
        Type type;
        void* storage = nullptr;

        std::optional<HandleSlim> parent;
        std::vector<HandleSlim> children;

        bool shouldSerializeBinary = true;
        bool shouldSerializeJson = true;

        RelicMetadata() = default;
        RelicMetadata(
            RelicID id,
            Openness openness,
            Locality locality,
            Type type,
            void* storage,
            bool shouldSerializeBinary,
            bool shouldSerializeJson);
    };
}