#pragma once

#include <Arca/Curator.h>

#include "BasicCommand.h"

using namespace Arca;

class DifferentiableCuratorBase : public Curator
{
public:
    bool shouldAbort = false;
    std::function<void(DifferentiableCuratorBase&)> onCommand;

    virtual ~DifferentiableCuratorBase() = 0;

    Reliquary& TheOwner();

    template<class RelicT, std::enable_if_t<is_relic_v<RelicT>, int> = 0>
    RelicT* TheData(RelicID id);
    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
    ShardT* TheData(RelicID id);

    [[nodiscard]] virtual size_t Differentiator() const = 0;
public:
    explicit DifferentiableCuratorBase(Init init);

    void Handle(const BasicCommand& command, Stage& stage);
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
        static constexpr ObjectType objectType = ObjectType::Curator;
        static const inline TypeName typeName = "DifferentiableCurator<" + Chroma::ToString(differentiator) + ">";
        using HandledCommands = HandledCommands<BasicCommand>;
    };
}

namespace Inscription
{
    template<size_t differentiator, class Archive>
    struct ScribeTraits<DifferentiableCurator<differentiator>, Archive> final
    {
        using Category = ArcaNullScribeCategory<DifferentiableCurator<differentiator>>;
    };
}