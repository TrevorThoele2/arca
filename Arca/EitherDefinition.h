#pragma once

#include "EitherImplementation.h"
#include "Reliquary.h"

namespace Arca
{
    template<class T>
    bool MatrixImplementation<Either<T>>::Contains(RelicID id, const Reliquary& reliquary)
    {
        return reliquary.Contains<T>(id) || reliquary.Contains<const T>(id);
    }

    template<class T>
    void MatrixImplementation<Either<T>>::Destroy(RelicID id, Reliquary& reliquary)
    {
        reliquary.Do<Arca::Destroy<T>>(id);
        reliquary.Do<Arca::Destroy<const T>>(id);
    }

    template<class T>
    auto MatrixImplementation<Either<T>>::AllFrom(Reliquary& reliquary) -> std::vector<std::tuple<RelicID, Stored>>
    {
        auto nonConstBatch = reliquary.Batch<T>();
        auto constBatch = reliquary.Batch<const T>();
        std::vector<std::tuple<RelicID, Stored>> list;
        list.reserve(nonConstBatch.Size() + constBatch.Size());
        for (auto loop = nonConstBatch.begin(); loop != nonConstBatch.end(); ++loop)
            list.emplace_back(loop.ID(), &*loop);
        for (auto loop = constBatch.begin(); loop != constBatch.end(); ++loop)
            list.emplace_back(loop.ID(), &*loop);
        return list;
    }

    template<class T>
    auto MatrixImplementation<Either<T>>::CreateStored(RelicID id, Reliquary& reliquary) -> Stored
    {
        return CreateIndexValue(id, reliquary);
    }

    template<class T>
    auto MatrixImplementation<Either<T>>::ToBatchSourceReference(Stored& stored) -> BatchReference
    {
        return *stored;
    }

    template<class T>
    auto MatrixImplementation<Either<T>>::ToBatchSourceOptional(Stored& stored) -> BatchOptional
    {
        return stored;
    }

    template<class T>
    auto MatrixImplementation<Either<T>>::CreateIndexValue(RelicID id, Reliquary& reliquary)
        -> IndexOptional
    {
        auto found = reliquary.FindStorage<T>(id);
        if (found)
            return found;
        else
            return reliquary.FindStorage<const T>(id);
    }

    template<class T>
    bool MatrixContains<Either<T>>::Contains(Type type)
    {
        return type == TypeFor<T>() || type == TypeFor<const T>();
    }
}