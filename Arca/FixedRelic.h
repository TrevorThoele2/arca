#pragma once

#include <type_traits>

#include "RelicID.h"
#include "Handle.h"
#include "ShardTraits.h"
#include "Ptr.h"

namespace Arca
{
    class FixedRelic
    {
    public:
        operator Handle() const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] Ptr<ShardT> Find() const;
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] bool Has() const;

        void ParentTo(const Handle& parent) const;
        std::optional<Handle> Parent() const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Reliquary& Owner() const;
    private:
        RelicID id;
        Reliquary* owner;
    private:
        FixedRelic(RelicID id, Reliquary& owner);
    private:
        friend Reliquary;
    };
}
