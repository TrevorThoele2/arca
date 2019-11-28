#pragma once

#include "RelicID.h"
#include "Handle.h"
#include "ShardTraits.h"
#include "Ptr.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    class DynamicRelic
    {
    public:
        operator Handle() const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        Ptr<ShardT> Create();
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        void Destroy();
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
        DynamicRelic(RelicID id, Reliquary& owner);
    private:
        friend Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::Arca::DynamicRelic, BinaryArchive> final :
        public CompositeScribe<::Arca::DynamicRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };
}