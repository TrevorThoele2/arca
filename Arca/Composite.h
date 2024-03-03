#pragma once

#include "IsComposite.h"
#include "RelicID.h"
#include "PtrTypeFor.h"

#include <Chroma/VariadicTemplate.h>

namespace Arca
{
    class Reliquary;

    template<::Chroma::VariadicTemplateSize i>
    struct CompositeToTuple
    {
        template<class TupleT>
        static void Do(TupleT& tuple, RelicID id, Reliquary& owner)
        {
            using T = std::tuple_element_t<i, TupleT>;

            std::get<i>(tuple) = T(id, owner);
        }
    };

    template<::Chroma::VariadicTemplateSize i>
    struct CompositeTupleContainsValid
    {
        template<class TupleT>
        static bool Check(TupleT& tuple)
        {
            return std::get<i>(tuple);
        }
    };
}