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
