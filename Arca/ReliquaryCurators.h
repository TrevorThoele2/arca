#pragma once

#include "ReliquaryComponent.h"

#include <memory>
#include <unordered_map>

#include "CuratorHandle.h"

#include "KnownPolymorphicSerializer.h"

namespace Arca
{
    class ReliquaryCurators : public ReliquaryComponent
    {
    public:
        template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int> = 0>
        [[nodiscard]] Curator* Find(const TypeName& type);
        template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int> = 0>
        [[nodiscard]] const Curator* Find(const TypeName& type) const;
        template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        [[nodiscard]] CuratorT* Find();
        template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        [[nodiscard]] const CuratorT* Find() const;
    public:
        using HandlePtr = std::unique_ptr<CuratorHandle>;
        using Map = std::unordered_map<TypeName, HandlePtr>;
        Map map;

        using Stage = std::vector<Curator*>;
        using Pipeline = std::vector<Stage>;
        Pipeline workPipeline;

        KnownPolymorphicSerializerList serializers;

        template<class CuratorT>
        [[nodiscard]] bool Has() const;

        template<class Function>
        void Work(Function function);
    private:
        explicit ReliquaryCurators(Reliquary& owner);
        friend Reliquary;
    };
}
