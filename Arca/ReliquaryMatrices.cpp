#include "ReliquaryMatrices.h"

namespace Arca
{
    void ReliquaryMatrices::Clear()
    {
        knownList.clear();
        batchSources.Clear();
    }

    void ReliquaryMatrices::CreationTransaction::Finalize()
    {
        for (auto& matrix : knownMatrices)
            if (matrix->Exists(id, *owner))
                matrix->Created(id, *owner);
    }

    ReliquaryMatrices::CreationTransaction::CreationTransaction(
        std::vector<KnownMatrix*>&& knownMatrices, RelicID id, Reliquary& owner)
        :
        id(id), owner(&owner), knownMatrices(std::move(knownMatrices))
    {}

    void ReliquaryMatrices::DestroyingTransaction::Finalize(Type type)
    {
        for (auto& matrix : knownMatrices)
            if(matrix->Contains(type))
                matrix->Destroying(id, *owner);
    }

    ReliquaryMatrices::DestroyingTransaction::DestroyingTransaction(
        std::vector<KnownMatrix*> && knownMatrices, RelicID id, Reliquary& owner)
        :
        id(id), owner(&owner), knownMatrices(std::move(knownMatrices))
    {}

    auto ReliquaryMatrices::StartCreationTransaction(RelicID id) -> CreationTransaction
    {
        std::vector<KnownMatrix*> knownMatrices;
        for (auto& matrix : knownList)
            if (!matrix.value.Exists(id, Owner()))
                knownMatrices.push_back(&matrix.value);
        return CreationTransaction(std::move(knownMatrices), id, Owner());
    }

    auto ReliquaryMatrices::StartDestroyingTransaction(RelicID id) -> DestroyingTransaction
    {
        std::vector<KnownMatrix*> knownMatrices;
        for (auto& matrix : knownList)
            if (matrix.value.Exists(id, Owner()))
                knownMatrices.push_back(&matrix.value);
        return DestroyingTransaction(std::move(knownMatrices), id, Owner());
    }

    ReliquaryMatrices::BatchSources::BatchSources(ReliquaryMatrices& owner) : owner(&owner)
    {}

    ReliquaryMatrices::ReliquaryMatrices(Reliquary& owner) : ReliquaryComponent(owner, "matrix")
    {}
}