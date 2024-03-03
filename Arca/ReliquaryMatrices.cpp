#include "ReliquaryMatrices.h"

namespace Arca
{
    void ReliquaryMatrices::Clear()
    {
        knownList.clear();
        batchSources.Clear();
    }

    void ReliquaryMatrices::CreationSnapshotObject::Finalize()
    {
        for (auto& matrix : knownMatrices)
            if (matrix->Exists(id, *owner))
                matrix->Created(id, *owner);
    }

    ReliquaryMatrices::CreationSnapshotObject::CreationSnapshotObject(
        std::vector<KnownMatrix*>&& knownMatrices, RelicID id, Reliquary& owner)
        :
        id(id), owner(&owner), knownMatrices(std::move(knownMatrices))
    {}

    void ReliquaryMatrices::DestroyingSnapshotObject::Finalize(Type type)
    {
        for (auto& matrix : knownMatrices)
            if(matrix->Contains(type))
                matrix->Destroying(id, *owner);
    }

    ReliquaryMatrices::DestroyingSnapshotObject::DestroyingSnapshotObject(
        std::vector<KnownMatrix*> && knownMatrices, RelicID id, Reliquary& owner)
        :
        id(id), owner(&owner), knownMatrices(std::move(knownMatrices))
    {}

    auto ReliquaryMatrices::CreationSnapshot(RelicID id) -> CreationSnapshotObject
    {
        std::vector<KnownMatrix*> knownMatrices;
        for (auto& matrix : knownList)
            if (!matrix.value.Exists(id, Owner()))
                knownMatrices.push_back(&matrix.value);
        return CreationSnapshotObject(std::move(knownMatrices), id, Owner());
    }

    auto ReliquaryMatrices::DestroyingSnapshot(RelicID id) -> DestroyingSnapshotObject
    {
        std::vector<KnownMatrix*> knownMatrices;
        for (auto& matrix : knownList)
            if (matrix.value.Exists(id, Owner()))
                knownMatrices.push_back(&matrix.value);
        return DestroyingSnapshotObject(std::move(knownMatrices), id, Owner());
    }

    ReliquaryMatrices::BatchSources::BatchSources(ReliquaryMatrices& owner) : owner(&owner)
    {}

    ReliquaryMatrices::ReliquaryMatrices(Reliquary& owner) : ReliquaryComponent(owner, "matrix")
    {}
}