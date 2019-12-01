#pragma once

#include "TypedRelic.h"
#include "Reliquary.h"
#include "ExtractShards.h"

namespace Arca
{
    template<class Derived, class... AllShards>
    class TypedRelicAutomation : public TypedRelic
    {
    public:
        using Shards = ShardList<AllShards...>;
    protected:
        TypedRelicAutomation() = default;
        explicit TypedRelicAutomation(const ::Inscription::BinaryTableData<TypedRelic>& data)
            : TypedRelic(data)
        {}
    protected:
        [[nodiscard]] auto ExtractShards() const
        {
            return Arca::ExtractShards<Shards>(ID(), Owner());
        }
    protected:
        [[nodiscard]] bool ReliquaryContainsSelf() const override final
        {
            return Owner().Contains<Derived>(ID());
        }
    };
}