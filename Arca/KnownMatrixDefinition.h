#pragma once

#include "KnownMatrix.h"

#include "Reliquary.h"
#include "Index.h"
#include "MatrixFormed.h"
#include "MatrixDissolved.h"

namespace Arca
{
    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    KnownMatrix::KnownMatrix(Chroma::TypeIdentity<MatrixT>) :
        base(std::make_unique<Derived<MatrixT>>())
    {}

    template<class MatrixT>
    auto KnownMatrix::Derived<MatrixT>::Clone() const -> std::unique_ptr<Base>
    {
        return std::make_unique<Derived>(*this);
    }

    template<class MatrixT>
    void KnownMatrix::Derived<MatrixT>::Created(RelicID relicID, Reliquary& reliquary)
    {
        if (interactWithBatchSource)
            CreateBatchSourceEntry<MatrixT>(relicID, reliquary);
        if (interactWithSignals)
            SignalCreated<MatrixT>(relicID, reliquary);
    }

    template<class MatrixT>
    void KnownMatrix::Derived<MatrixT>::Destroying(RelicID relicID, Reliquary& reliquary)
    {
        if (interactWithBatchSource)
            RemoveBatchSourceEntry<MatrixT>(relicID, reliquary);
        if (interactWithSignals)
            SignalDestroying<MatrixT>(relicID, reliquary);
    }

    template<class MatrixT>
    bool KnownMatrix::Derived<MatrixT>::Exists(RelicID relicID, Reliquary& reliquary) const
    {
        return reliquary.Contains<MatrixT>(relicID);
    }

    template<class MatrixT>
    bool KnownMatrix::Derived<MatrixT>::Contains(Type type) const
    {
        return MatrixContains<MatrixT>::Contains(type);
    }

    template<class MatrixT>
    void KnownMatrix::Derived<MatrixT>::InteractWithBatchSource(bool value)
    {
        interactWithBatchSource = value;
    }

    template<class MatrixT>
    void KnownMatrix::Derived<MatrixT>::InteractWithSignals(bool value)
    {
        interactWithSignals = value;
    }

    template<class MatrixT>
    bool KnownMatrix::Derived<MatrixT>::IsInteractingWithAnything() const
    {
        return interactWithBatchSource || interactWithSignals;
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    void KnownMatrix::CreateBatchSourceEntry(RelicID relicID, Reliquary& reliquary)
    {
        auto& batchSource = reliquary.matrices.batchSources.Required<MatrixT>();
        if (!batchSource.Has(relicID))
            batchSource.Add({ relicID, MatrixImplementation<MatrixT>::CreateStored(relicID, reliquary) });
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    void KnownMatrix::RemoveBatchSourceEntry(RelicID relicID, Reliquary& reliquary)
    {
        auto& batchSource = reliquary.matrices.batchSources.Required<MatrixT>();
        batchSource.Remove(relicID);
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    void KnownMatrix::SignalCreated(RelicID relicID, Reliquary& reliquary)
    {
        reliquary.Raise(MatrixFormed<MatrixT>{reliquary.Find<MatrixT>(relicID)});
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    void KnownMatrix::SignalDestroying(RelicID relicID, Reliquary& reliquary)
    {
        reliquary.Raise(MatrixDissolved<MatrixT>{reliquary.Find<MatrixT>(relicID)});
    }
}