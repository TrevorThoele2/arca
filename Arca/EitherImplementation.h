#pragma once

#include "MatrixImplementation.h"
#include "Either.h"

#include "RelicID.h"

namespace Arca
{
    class Reliquary;

    template<class T>
    class MatrixImplementation<Either<T>>
    {
    public:
        static bool Contains(RelicID id, const Reliquary& reliquary);
        static void Destroy(RelicID id, Reliquary& reliquary);
    public:
        using Stored = const T*;

        using BatchReference = const T&;
        using BatchOptional = const T*;

        static std::vector<std::tuple<RelicID, Stored>> AllFrom(Reliquary& reliquary);
        static Stored CreateStored(RelicID id, Reliquary& reliquary);

        static BatchReference ToBatchSourceReference(Stored& stored);
        static BatchOptional ToBatchSourceOptional(Stored& stored);
    public:
        using PtrReference = const T&;
        using PtrOptional = const T*;

        static PtrOptional CreatePtrValue(RelicID id, Reliquary& reliquary);
    };
}