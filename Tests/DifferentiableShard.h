#pragma once

#include <Arca/Shard.h>

template<size_t i>
class DifferentiableShard
{
public:
    int value = 0;
public:
    DifferentiableShard() = default;
    explicit DifferentiableShard(int value);
};

template<size_t i>
DifferentiableShard<i>::DifferentiableShard(int value) :
    value(value)
{}

namespace Arca
{
    template<size_t i>
    struct Traits<DifferentiableShard<i>>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "DifferentiableShard" + Chroma::ToString(i);
    };
}

namespace Inscription
{
    template<size_t i>
    class Scribe<DifferentiableShard<i>> final
    {
    public:
        using ObjectT = DifferentiableShard<i>;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {
            archive("value", object.value);
        }
    };

    template<size_t i, class Archive>
    struct ScribeTraits<DifferentiableShard<i>, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<DifferentiableShard<i>>;
    };
}