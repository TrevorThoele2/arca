#pragma once

#include "ReliquaryMatrices.h"

#include "EitherDefinition.h"
#include "AllDefinition.h"

namespace Arca
{
    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    void ReliquaryMatrices::Destroy(RelicID id)
    {
        MatrixImplementation<MatrixT>::Destroy(id, *owner);
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    bool ReliquaryMatrices::Contains(RelicID id) const
    {
        return MatrixImplementation<MatrixT>::Contains(id, *owner);
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    void ReliquaryMatrices::EnsureInteraction(void(KnownMatrix::*interaction)(bool))
    {
        auto known = EnsuredKnown<MatrixT>();
        ((known->value).*interaction)(true);
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    void ReliquaryMatrices::EnsureStopInteraction(void(KnownMatrix::*stopInteraction)(bool))
    {
        auto known = EnsuredKnown<MatrixT>();
        ((known->value).*stopInteraction)(false);

        if (!known->value.IsInteractingWithAnything())
            knownList.erase(known);
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    auto ReliquaryMatrices::EnsuredKnown() -> KnownList::iterator
    {
        const auto type = std::type_index(typeid(MatrixT));

        auto found = std::find_if(
            knownList.begin(),
            knownList.end(),
            [type](const StoredKnown& stored)
            {
                return type == stored.type;
            });
        if (found != knownList.end())
            return found;

        knownList.push_back(
            StoredKnown
            {
                KnownMatrix(Chroma::TypeIdentity<MatrixT>{}),
                type
            });
        return --knownList.end();
    }

    template<class T>
    [[nodiscard]] TypeName ReliquaryMatrices::BatchSources::KeyFor()
    {
        return TypeFor<T>().name;
    }
}