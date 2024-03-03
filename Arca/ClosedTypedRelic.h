#pragma once

#include "RelicInit.h"
#include "Reliquary.h"
#include "TypeFor.h"

namespace Arca
{
    template<class Derived>
    class ClosedTypedRelic
    {
    public:
        [[nodiscard]] std::optional<Handle> Parent() const
        {
            return owner->ParentOf(Handle(ID(), Owner(), TypeFor<Derived>(), HandleObjectType::Relic));
        }

        [[nodiscard]] RelicID ID() const
        {
            return id;
        }

        [[nodiscard]] Reliquary& Owner() const
        {
            return *owner;
        }
    protected:
        using Init = RelicInit;

        explicit ClosedTypedRelic(Init init) :
            id(init.id), owner(&init.owner)
        {}

        ClosedTypedRelic(const ClosedTypedRelic& arg) = default;
        ClosedTypedRelic(ClosedTypedRelic&& arg) noexcept = default;

        ClosedTypedRelic& operator=(const ClosedTypedRelic& arg) = default;
        ClosedTypedRelic& operator=(ClosedTypedRelic&& arg) noexcept = default;
    protected:
        template<class ShardT, class... ConstructorArgs, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        Index<ShardT> Create(ConstructorArgs&& ... constructorArgs) const
        {
            return Owner().template CreateFromInternal<ShardT>(ID(), std::forward<ConstructorArgs>(constructorArgs)...);
        }

        template<class ShardT, class... ConstructorArgs, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        Index<ShardT> FindOrCreate(ConstructorArgs&& ... constructorArgs) const
        {
            auto found = Find<ShardT>();
            if (found)
                return found;

            return Create<ShardT>(std::forward<ConstructorArgs>(constructorArgs)...);
        }

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] Index<ShardT> Find() const
        {
            return Index<ShardT>(ID(), Owner());
        }
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
    private:
        template<class, class>
        friend class BatchSource;
    };
}