#pragma once

#include "RelicID.h"
#include "Openness.h"
#include "Handle.h"
#include "ShardTraits.h"
#include "Either.h"
#include "CompositeTraits.h"

#include "Ptr.h"

#include "CompositeScribe.h"

namespace Arca
{
    class ClosedRelic
    {
    public:
        explicit operator bool() const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] Ptr<ShardT> Find() const;
        template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int> = 0>
        [[nodiscard]] Ptr<typename EitherT::ShardT> Find() const;
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] bool Contains() const;
        template<class EitherT, std::enable_if_t<is_either_v<EitherT>, int> = 0>
        [[nodiscard]] bool Contains() const;
        template<class ShardsT, std::enable_if_t<is_composite_v<ShardsT>, int> = 0>
        [[nodiscard]] bool Contains() const;

        [[nodiscard]] std::optional<Handle> Parent() const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Reliquary& Owner() const;
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
    private:
        ClosedRelic() = default;
    private:
        friend Reliquary;
        friend class ReliquaryRelics;
        friend class ReliquaryOrigin;
        template<class, class>
        friend class BatchSource;
    private:
        INSCRIPTION_ACCESS;
    };

    template<>
    struct Traits<ClosedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const Openness openness = Openness::Closed;
        static const TypeName typeName;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::Arca::ClosedRelic, BinaryArchive> final :
        public ArcaCompositeScribe<::Arca::ClosedRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };
}