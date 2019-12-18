#pragma once

#include "RelicID.h"
#include "RelicStructure.h"
#include "RelicTraits.h"
#include "Handle.h"
#include "Either.h"
#include "AreAllShards.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;
    class ReliquaryRelics;
    class OpenTypedRelic;

    class OpenTypedRelic
    {
    public:
        explicit operator bool() const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        ShardT* Create();
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        void Destroy();
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardT* Find() const;
        template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int> = 0>
        [[nodiscard]] typename EitherT::ShardT* Find() const;
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] bool Contains() const;
        template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int> = 0>
        [[nodiscard]] bool Contains() const;
        template<class... ShardsT, std::enable_if_t<are_all_shards_v<ShardsT...> && (sizeof...(ShardsT) > 1), int> = 0>
        [[nodiscard]] bool Contains() const;

        [[nodiscard]] std::optional<Handle> Parent() const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Reliquary& Owner() const;
    public:
        virtual ~OpenTypedRelic() = 0;

        void Initialize(Reliquary& owner);
    protected:
        OpenTypedRelic() = default;

        virtual void InitializeImplementation() {}
        [[nodiscard]] virtual bool ReliquaryContainsSelf() const = 0;
        [[nodiscard]] virtual Type Type() const = 0;
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
    private:
        friend class ReliquaryRelics;
        friend class ReliquaryOrigin;
        template<class, class>
        friend class BatchSource;
    private:
        INSCRIPTION_ACCESS;
    };
}