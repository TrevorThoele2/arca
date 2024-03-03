#pragma once

#include <Chroma/VariadicTemplate.h>
#include "Traits.h"
#include "TypeFor.h"
#include "Index.h"

namespace Arca
{
    template<class T>
    struct TransformToReference
    {
        using Type = Index<T>;
    };

    template<class... Ts>
    struct All
    {
        using Pack = ::Chroma::VariadicTemplate<Ts...>;
        using ReferenceTuple = typename Pack::template Transform<TransformToReference>::Type::TupleT;
    };

    template<Chroma::VariadicTemplateSize i>
    struct AllTypeNameIterator
    {
        template<class Pack>
        static std::string Do(const std::string& baseString)
        {
            using T = typename Pack::template Parameter<i - 1>::Type;

            return AllTypeNameIterator<i - 1>::template Do<Pack>(
                i == Pack::count
                    ? baseString + Chroma::ToString(TypeFor<T>())
                    : baseString + "," + Chroma::ToString(TypeFor<T>()));
        }
    };

    template<>
    struct AllTypeNameIterator<0>
    {
        template<class Pack>
        static std::string Do(const std::string& string)
        {
            return string;
        }
    };

    template<class... Ts>
    struct Traits<All<Ts...>>
    {
    private:
        using Pack = typename All<Ts...>::Pack;
    public:
        static const ObjectType objectType = ObjectType::Matrix;
        static TypeName TypeName()
        {
            return "Arca::All<" + AllTypeNameIterator<Pack::count>::template Do<Pack>("") + ">";
        }
    };
}