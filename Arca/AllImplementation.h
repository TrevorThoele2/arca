#pragma once

#include "MatrixImplementation.h"
#include "All.h"

#include "RelicID.h"

namespace Arca
{
    class Reliquary;

    template<Chroma::VariadicTemplateSize i>
    struct AllToTuple
    {
        template<class TupleT>
        static void Do(TupleT& tuple, RelicID id, Reliquary& owner)
        {
            using T = std::tuple_element_t<i, TupleT>;

            std::get<i>(tuple) = T(id, owner);
        }
    };

    template<Chroma::VariadicTemplateSize i>
    struct AllTupleContainsValid
    {
        template<class TupleT>
        static bool Check(TupleT& tuple)
        {
            return std::get<i>(tuple);
        }
    };

    template<Chroma::VariadicTemplateSize i>
    struct ContainsAllShardsIterator
    {
        template<class Pack>
        static bool Check(Pack, std::unordered_set<Type> types)
        {
            using T = typename Pack::template Parameter<i>::Type;

            if constexpr (is_shard_v<T>)
                return types.find(TypeFor<T>()) != types.end();
            else
                return MatrixImplementation<T>::IsIn(types);
        }
    };

    template<Chroma::VariadicTemplateSize i>
    struct AllTypesIterator
    {
        template<class Pack>
        static void Do(Pack, std::unordered_set<Type>& types)
        {
            using T = typename Pack::template Parameter<i>::Type;

            types.insert(TypeFor<T>());
        }
    };

    template<Chroma::VariadicTemplateSize i>
    struct ContainsTypeIterator
    {
        template<class Pack>
        static bool Check(Pack, Type type)
        {
            using T = typename Pack::template Parameter<i>::Type;

            return MatrixContains<T>::Contains(type);
        }
    };

    template<class... Ts>
    class MatrixImplementation<All<Ts...>>
    {
    public:
        static bool IsIn(std::unordered_set<Type> types);
        static std::unordered_set<Type> Types();
    public:
        using Tuple = typename All<Ts...>::Pack::template Transform<TransformToReference>::Type::TupleT;
    public:
        using Stored = Tuple;

        using BatchReference = Tuple&;
        using BatchOptional = std::optional<Tuple>;

        static std::vector<std::tuple<RelicID, Stored>> AllFrom(Reliquary& reliquary);
        static Stored CreateStored(RelicID id, Reliquary& reliquary);

        static BatchReference ToBatchSourceReference(Stored& stored);
        static BatchOptional ToBatchSourceOptional(Stored& stored);
    public:
        using IndexReference = Tuple;
        using IndexOptional = std::optional<Tuple>;

        static IndexOptional CreateIndexValue(RelicID id, Reliquary& reliquary);
        static IndexOptional DefaultIndexValue();
    private:
        using Pack = typename All<Ts...>::Pack;
    };

    template<class... Ts>
    struct MatrixContains<All<Ts...>>
    {
        static bool Contains(Type type);
    private:
        using Pack = typename All<Ts...>::Pack;
    };

    template<class... Ts>
    bool MatrixImplementation<All<Ts...>>::IsIn(std::unordered_set<Type> types)
    {
        return Chroma::IterateRangeCheckStop<
            Chroma::VariadicTemplateSize,
            ContainsAllShardsIterator,
            bool,
            Pack::count - 1>

            (false, Pack{}, types);
    }

    template<class... Ts>
    std::unordered_set<Type> MatrixImplementation<All<Ts...>>::Types()
    {
        auto types = std::unordered_set<Type>();
        Chroma::IterateRange<Chroma::VariadicTemplateSize, AllTypesIterator, Pack::count - 1>(Pack{}, types);
        return types;
    }

    template<class... Ts>
    auto MatrixImplementation<All<Ts...>>::AllFrom(Reliquary& reliquary) -> std::vector<std::tuple<RelicID, Stored>>
    {
        std::vector<std::tuple<RelicID, Stored>> list;
        for (auto id : reliquary.AllIDs())
        {
            auto potential = CreateIndexValue(id, reliquary);
            if (potential)
                list.emplace_back(id, *potential);
        }
        return list;
    }

    template<class... Ts>
    auto MatrixImplementation<All<Ts...>>::CreateStored(RelicID id, Reliquary& reliquary) -> Stored
    {
        return *CreateIndexValue(id, reliquary);
    }

    template<class... Ts>
    auto MatrixImplementation<All<Ts...>>::ToBatchSourceReference(Stored& stored) -> BatchReference
    {
        return stored;
    }

    template<class... Ts>
    auto MatrixImplementation<All<Ts...>>::ToBatchSourceOptional(Stored& stored) -> BatchOptional
    {
        return stored;
    }

    template<class... Ts>
    auto MatrixImplementation<All<Ts...>>::CreateIndexValue(RelicID id, Reliquary& reliquary) -> IndexOptional
    {
        Tuple tuple;
        Chroma::IterateRange<Chroma::VariadicTemplateSize, AllToTuple, sizeof...(Ts) - 1>
            (tuple, id, reliquary);
        auto allValid = Chroma::IterateRangeCheckStop<Chroma::VariadicTemplateSize, AllTupleContainsValid, bool, sizeof...(Ts) - 1>
            (false, tuple);
        return !allValid ? IndexOptional{} : tuple;
    }

    template<class... Ts>
    auto MatrixImplementation<All<Ts...>>::DefaultIndexValue() -> IndexOptional
    {
        return {};
    }

    template<class... Ts>
    bool MatrixContains<All<Ts...>>::Contains(Type type)
    {
        return Chroma::IterateRangeCheckStop<
            Chroma::VariadicTemplateSize,
            ContainsTypeIterator,
            bool,
            Pack::count - 1>

            (true, Pack{}, type);
    }
}