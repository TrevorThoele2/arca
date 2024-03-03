#pragma once

#include <Arca/Curator.h>

using namespace Arca;

class DifferentiableCuratorBase : public Curator
{
public:
    bool shouldAbort = false;
    std::function<void(DifferentiableCuratorBase&)> onWork;

    virtual ~DifferentiableCuratorBase() = 0;

    Reliquary& TheOwner();

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
    RelicT* TheData(RelicID id);
    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
    ShardT* TheData(RelicID id);

    [[nodiscard]] virtual size_t Differentiator() const = 0;
public:
    explicit DifferentiableCuratorBase(Init init);

    void Work(Stage& stage);
};

template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int>>
RelicT* DifferentiableCuratorBase::TheData(RelicID id)
{
    return MutablePointer().Of<RelicT>(id);
}

template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
ShardT* DifferentiableCuratorBase::TheData(RelicID id)
{
    return MutablePointer().Of<ShardT>(id);
}

template<size_t differentiator>
class DifferentiableCurator : public DifferentiableCuratorBase
{
public:
    static std::function<void(DifferentiableCurator&)> onConstructor;

    [[nodiscard]] size_t Differentiator() const override
    {
        return differentiator;
    }
public:
    explicit DifferentiableCurator(Init init);
};

template<size_t differentiator>
std::function<void(DifferentiableCurator<differentiator>&)>
    DifferentiableCurator<differentiator>::onConstructor = [](DifferentiableCurator<differentiator>&) {};

template<size_t differentiator>
DifferentiableCurator<differentiator>::DifferentiableCurator(Init init) : DifferentiableCuratorBase(init)
{
    onConstructor(*this);
    onConstructor = [](DifferentiableCurator&) {};
}

using DerivedCurator = DifferentiableCurator<0>;

namespace Arca
{
    template<size_t differentiator>
    struct Traits<DifferentiableCurator<differentiator>>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "DifferentiableCurator<" + ::Chroma::ToString(differentiator) + ">";
    };
}

namespace Inscription
{
    template<size_t differentiator>
    class Scribe<DifferentiableCurator<differentiator>, BinaryArchive>
        : public ArcaNullScribe<DifferentiableCurator<differentiator>, BinaryArchive>
    {};
}