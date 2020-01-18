#pragma once

#include "AllImplementation.h"
#include "Reliquary.h"

namespace Arca
{
    template<::Chroma::VariadicTemplateSize i>
    struct AllToTuple
    {
        template<class TupleT>
        static void Do(TupleT& tuple, RelicID id, Reliquary& owner)
        {
            using T = std::tuple_element_t<i, TupleT>;

            std::get<i>(tuple) = T(id, owner);
        }
    };

    template<::Chroma::VariadicTemplateSize i>
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
        static bool Check(Pack, RelicID id, const Reliquary& reliquary)
        {
            using T = typename Pack::template Parameter<i>::Type;

            return reliquary.Contains<T>(id);
        }
    };

    template<Chroma::VariadicTemplateSize i>
    struct DestroyAllShardsIterator
    {
        template<class Pack>
        static void Do(Pack, RelicID id, Reliquary& reliquary)
        {
            using T = typename Pack::template Parameter<i>::Type;

            reliquary.Destroy<T>(id);
        }
    };

    template<class... Ts>
    bool MatrixImplementation<All<Ts...>>::Contains(RelicID id, const Reliquary& reliquary)
    {
        return ::Chroma::IterateRangeCheckStop<
            ::Chroma::VariadicTemplateSize,
            ContainsAllShardsIterator,
            bool,
            Pack::count - 1>

            (false, Pack{}, id, reliquary);
    }

    template<class... Ts>
    void MatrixImplementation<All<Ts...>>::Destroy(RelicID id, Reliquary& reliquary)
    {
        return ::Chroma::IterateRange<
            ::Chroma::VariadicTemplateSize,
            DestroyAllShardsIterator,
            Pack::count - 1>

            (Pack{}, id, reliquary);
    }

    template<class... Ts>
    auto MatrixImplementation<All<Ts...>>::AllFrom(Reliquary& reliquary) -> std::vector<std::tuple<RelicID, Stored>>
    {
        std::vector<std::tuple<RelicID, Stored>> list;
        for (auto id : reliquary.AllIDs())
        {
            auto potential = CreatePtrValue(id, reliquary);
            if (potential)
                list.emplace_back(id, *potential);
        }
        return list;
    }

    template<class... Ts>
    auto MatrixImplementation<All<Ts...>>::CreateStored(RelicID id, Reliquary& reliquary) -> Stored
    {
        return *CreatePtrValue(id, reliquary);
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
    auto MatrixImplementation<All<Ts...>>::CreatePtrValue(RelicID id, Reliquary& reliquary) -> PtrOptional
    {
        Tuple tuple;
        Chroma::IterateRange<Chroma::VariadicTemplateSize, AllToTuple, sizeof...(Ts) - 1>
            (tuple, id, reliquary);
        auto allValid = Chroma::IterateRangeCheckStop<Chroma::VariadicTemplateSize, AllTupleContainsValid, bool, sizeof...(Ts) - 1>
            (false, tuple);
        if (!allValid)
            return {};
        else
            return tuple;
    }
}