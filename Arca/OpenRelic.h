#pragma once

#include "RelicID.h"
#include "Handle.h"
#include "ShardTraits.h"
#include "Ptr.h"
#include "Either.h"
#include "AreAllShards.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    class OpenRelic
    {
    public:
        operator Handle() const;
        explicit operator bool() const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        Ptr<ShardT> Create();
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        void Destroy();
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] Ptr<ShardT> Find() const;
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] bool Contains() const;
        template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int> = 0>
        [[nodiscard]] bool Contains() const;
        template<class... ShardsT, std::enable_if_t<are_all_shards_v<ShardsT...> && (sizeof...(ShardsT) > 1), int> = 0>
        [[nodiscard]] bool Contains() const;

        [[nodiscard]] std::optional<Handle> Parent() const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Reliquary& Owner() const;
    private:
        RelicID id;
        Reliquary* owner;
    private:
        OpenRelic(RelicID id, Reliquary& owner);
    private:
        friend Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };

    template<>
    struct Traits<OpenRelic>
    {
        static const TypeHandleName typeName;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::Arca::OpenRelic, BinaryArchive> final :
        public CompositeScribe<::Arca::OpenRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };
}