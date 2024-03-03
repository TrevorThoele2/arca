#pragma once

#include <optional>
#include <vector>

#include "RelicID.h"
#include "RelicOpenness.h"
#include "TypeHandle.h"

#include "Serialization.h"

namespace Arca
{
    struct RelicMetadata
    {
        RelicID id = 0;
        RelicOpenness openness = RelicOpenness::Open;
        std::optional<TypeHandle> typeHandle;
        void* storage = nullptr;

        std::optional<RelicID> parent;
        std::vector<RelicID> children;

        RelicMetadata() = default;
        RelicMetadata(
            RelicID id,
            RelicOpenness openness,
            std::optional<TypeHandle> typeHandle = {},
            void* storage = nullptr);
    };
}

namespace Inscription
{
    template<>
    class Scribe<::Arca::RelicMetadata, BinaryArchive> final :
        public CompositeScribe<::Arca::RelicMetadata, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };
}