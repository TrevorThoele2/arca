#pragma once

#include "MatrixImplementation.h"
#include "All.h"

#include "RelicID.h"

namespace Arca
{
    class Reliquary;

    template<class... Ts>
    class MatrixImplementation<All<Ts...>>
    {
    public:
        static bool Contains(RelicID id, const Reliquary& reliquary);
        static void Destroy(RelicID id, Reliquary& reliquary);
    public:
        using Tuple = typename All<Ts...>::Pack::template Transform<TransformToIndex>::Type::TupleT;
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
    private:
        using Pack = typename All<Ts...>::Pack;
    };
}