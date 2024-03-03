#pragma once

#include <optional>
#include <vector>

#include "VesselID.h"
#include "VesselDynamism.h"
#include "TypeHandle.h"

#include "Serialization.h"

namespace Arca
{
    struct VesselMetadata
    {
        VesselID id = 0;
        VesselDynamism dynamism = VesselDynamism::Dynamic;
        std::optional<TypeHandle> typeHandle;

        std::optional<VesselID> parent;
        std::vector<VesselID> children;

        VesselMetadata() = default;
        VesselMetadata(
            VesselID id,
            VesselDynamism dynamism,
            std::optional<TypeHandle> typeHandle = {});
    };
}

namespace Inscription
{
    template<>
    class Scribe<::Arca::VesselMetadata, BinaryArchive> final :
        public CompositeScribe<::Arca::VesselMetadata, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };
}