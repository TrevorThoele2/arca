#pragma once

#include "Command.h"

#include "IsRelic.h"
#include "IsShard.h"
#include "IsMatrix.h"
#include "RelicID.h"
#include "Index.h"

#include "Reliquary.h"

namespace Arca
{
    template<class T, class Enable = void>
    struct Destroy;

    template<class T>
    struct Destroy<T, std::enable_if_t<is_relic_v<T>>>
    {
        RelicID id;

        explicit Destroy(RelicID id);
        explicit Destroy(Index<T> index);

        void Do(Reliquary& reliquary) const;
    };

    template<class T>
    Destroy<T, std::enable_if_t<is_relic_v<T>>>::Destroy(RelicID id) : id(id)
    {}

    template<class T>
    Destroy<T, std::enable_if_t<is_relic_v<T>>>::Destroy(Index<T> index) : id(index.ID())
    {}

    template<class T>
    void Destroy<T, std::enable_if_t<is_relic_v<T>>>::Do(Reliquary& reliquary) const
    {
        reliquary.relics.Destroy<T>(id);
    }

    template<class T>
    struct Traits<Destroy<T, std::enable_if_t<is_relic_v<T>>>>
    {
        static constexpr ObjectType objectType = ObjectType::Command;
        static constexpr TypeName TypeName() { return "Arca::Destroy<" + Chroma::ToString(TypeFor<T>()) + ">"; }
        static constexpr bool selfContained = true;
    };

    template<class T>
    struct Destroy<T, std::enable_if_t<is_shard_v<T>>>
    {
        RelicID id;

        explicit Destroy(RelicID id);
        explicit Destroy(Index<T> index);

        void Do(Reliquary& reliquary) const;
    };

    template<class T>
    Destroy<T, std::enable_if_t<is_shard_v<T>>>::Destroy(RelicID id) : id(id)
    {}

    template<class T>
    Destroy<T, std::enable_if_t<is_shard_v<T>>>::Destroy(Arca::Index<T> index) : id(index.ID())
    {}

    template<class T>
    void Destroy<T, std::enable_if_t<is_shard_v<T>>>::Do(Reliquary& reliquary) const
    {
        reliquary.shards.TransactionalDestroy<T>(id);
    }

    template<class T>
    struct Traits<Destroy<T, std::enable_if_t<is_shard_v<T>>>>
    {
        static constexpr ObjectType objectType = ObjectType::Command;
        static constexpr TypeName TypeName() { return "Arca::Destroy<" + Chroma::ToString(TypeFor<T>()) + ">"; }
        static constexpr bool selfContained = true;
    };

    template<class T>
    struct Destroy<T, std::enable_if_t<is_matrix_v<T>>>
    {
        RelicID id;

        explicit Destroy(RelicID id);

        void Do(Reliquary& reliquary) const;
    };

    template<class T>
    Destroy<T, std::enable_if_t<is_matrix_v<T>>>::Destroy(RelicID id) : id(id)
    {}

    template<class T>
    void Destroy<T, std::enable_if_t<is_matrix_v<T>>>::Do(Reliquary& reliquary) const
    {
        reliquary.shards.Destroy<T>(id);
    }

    template<class T>
    struct Traits<Destroy<T, std::enable_if_t<is_matrix_v<T>>>>
    {
        static constexpr ObjectType objectType = ObjectType::Command;
        static constexpr TypeName TypeName() { return "Arca::Destroy<" + Chroma::ToString(TypeFor<T>()) + ">"; }
        static constexpr bool selfContained = true;
    };

    template<>
    struct Destroy<Handle>
    {
        Handle handle;

        explicit Destroy(Handle handle);
    };

    template<>
    struct Traits<Destroy<Handle>>
    {
        static constexpr ObjectType objectType = ObjectType::Command;
        static constexpr TypeName TypeName() { return "Arca::Destroy<Handle>"; }
    };
}