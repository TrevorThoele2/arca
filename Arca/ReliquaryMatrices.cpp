#include "ReliquaryMatrices.h"

namespace Arca
{
    void ReliquaryMatrices::Clear()
    {
        storedList.clear();
        batchSources.Clear();
    }

    void ReliquaryMatrices::ShardCreated(RelicID id, const Type& type, const std::unordered_set<Type>& allTypes)
    {
        for (auto& stored : storedList)
            if (stored.Contains(type) && stored.Exists(allTypes))
                stored.Created(id, batchSources, *owner, *signals);
    }

    void ReliquaryMatrices::ShardDestroying(RelicID id, const Type& type, const std::unordered_set<Type>& allTypes)
    {
        for (auto& stored : storedList)
            if (stored.Contains(type) && stored.Exists(allTypes))
                stored.Destroying(id, batchSources, *owner, *signals);
    }

    ReliquaryMatrices::BatchSources::BatchSources(ReliquaryMatrices& owner) : owner(&owner)
    {}

    ReliquaryMatrices::ReliquaryMatrices(Reliquary& owner, ReliquarySignals& signals) :
        owner(&owner), signals(&signals)
    {}

    void ReliquaryMatrices::Stored::Created(
        RelicID relicID, BatchSources& batchSources, Reliquary& reliquary, ReliquarySignals& signals)
    {
        if (base)
            base->Created(relicID, batchSources, reliquary, signals);
    }

    void ReliquaryMatrices::Stored::Destroying(
        RelicID relicID, BatchSources& batchSources, Reliquary& reliquary, ReliquarySignals& signals)
    {
        if (base)
            base->Destroying(relicID, batchSources, reliquary, signals);
    }

    bool ReliquaryMatrices::Stored::Exists(const std::unordered_set<Type>& shards) const
    {
        return !base ? false : base->Exists(shards);
    }

    bool ReliquaryMatrices::Stored::Contains(Type type) const
    {
        return !base ? false : base->Contains(type);
    }

    void ReliquaryMatrices::Stored::InteractWithBatchSource(bool value)
    {
        if (base)
            base->InteractWithBatchSource(value);
    }

    void ReliquaryMatrices::Stored::InteractWithSignals(bool value)
    {
        if (base)
            base->InteractWithSignals(value);
    }

    bool ReliquaryMatrices::Stored::IsInteractingWithAnything() const
    {
        return !base ? false : base->IsInteractingWithAnything();
    }

    std::type_index ReliquaryMatrices::Stored::MatrixType() const
    {
        return matrixType;
    }

    ReliquaryMatrices::Stored::Base::~Base() = default;
}