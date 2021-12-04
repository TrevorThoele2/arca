#pragma once

#include <type_traits>
#include "MatrixImplementation.h"
#include "IsShard.h"
#include "Traits.h"
#include "Reliquary.h"

namespace Arca
{
    class Reliquary;

    template<class T>
    struct Either
    {
        using ShardT = const std::decay_t<T>;
        using BareT = std::decay_t<T>;

        static_assert(is_shard_v<T>, "Eithers must wrap a Shard");
    };

    template<class T>
    struct Traits<Either<T>>
    {
        static constexpr ObjectType objectType = ObjectType::Matrix;
        static constexpr TypeName TypeName() { return "Arca::Either<" + Traits<std::decay_t<T>>::TypeName() + ">"; }
    };

    template<class T>
    class MatrixImplementation<Either<T>>
    {
    public:
        static bool IsIn(std::unordered_set<Type> types);
        static std::unordered_set<Type> Types();
    public:
        using Stored = const T*;

        using BatchReference = const T&;
        using BatchOptional = const T*;

        static std::vector<std::tuple<RelicID, Stored>> AllFrom(Reliquary& reliquary);
        static Stored CreateStored(RelicID id, Reliquary& reliquary);

        static BatchReference ToBatchSourceReference(Stored& stored);
        static BatchOptional ToBatchSourceOptional(Stored& stored);
    public:
        using IndexReference = const T&;
        using IndexOptional = const T*;

        static IndexOptional CreateIndexValue(RelicID id, Reliquary& reliquary);
        static IndexOptional DefaultIndexValue();
    };

    template<class T>
    bool MatrixImplementation<Either<T>>::IsIn(std::unordered_set<Type> types)
    {
        return types.find(TypeFor<T>()) != types.end() || types.find(TypeFor<const T>()) != types.end();
    }

    template<class T>
    std::unordered_set<Type> MatrixImplementation<Either<T>>::Types()
    {
        return { TypeFor<T>(), TypeFor<const T>() };
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
        return found ? found : reliquary.FindStorage<const T>(id);
    }

    template<class T>
    auto MatrixImplementation<Either<T>>::DefaultIndexValue() -> IndexOptional
    {
        return nullptr;
    }

    template<class T>
    struct MatrixContains<Either<T>>
    {
        static bool Contains(Type type);
    };

    template<class T>
    bool MatrixContains<Either<T>>::Contains(Type type)
    {
        return type == TypeFor<T>() || type == TypeFor<const T>();
    }
}