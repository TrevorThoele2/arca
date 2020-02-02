#pragma once

#include "RelicInitialization.h"
#include "Reliquary.h"
#include "TypeFor.h"

namespace Arca
{
    template<class Derived>
    class OpenTypedRelic
    {
    public:
        template<class ShardT, class... ConstructorArgs, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        ShardIndex<ShardT> Create(ConstructorArgs&& ... constructorArgs) const
        {
            return owner->Do<Arca::Create<ShardT>>(id, std::forward<ConstructorArgs>(constructorArgs)...);
        }

        template<class ShardT, class... ConstructorArgs, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        ShardIndex<ShardT> FindOrCreate(ConstructorArgs&& ... constructorArgs) const
        {
            auto found = Find<ShardT>();
            if (found)
                return found;

            return Create<ShardT>(std::forward<ConstructorArgs>(constructorArgs)...);
        }

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        void Destroy() const
        {
            owner->Do<Arca::Destroy<ShardT>>(id);
        }

        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        void Destroy() const
        {
            owner->Do<Arca::Destroy<MatrixT>>(id);
        }

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardIndex<ShardT> Find() const
        {
            return Arca::ShardIndex<ShardT>(id, *owner);
        }

        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        [[nodiscard]] MatrixIndex<MatrixT> Find() const
        {
            return Arca::MatrixIndex<MatrixT>(id, *owner);
        }

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] bool Contains() const
        {
            return static_cast<bool>(Find<ShardT>());
        }

        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        [[nodiscard]] bool Contains() const
        {
            return owner->Contains<MatrixT>(id);
        }

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
        using Initialization = RelicInitialization;

        explicit OpenTypedRelic(Initialization initialization) :
            id(initialization.id), owner(&initialization.owner)
        {}

        OpenTypedRelic(const OpenTypedRelic& arg) = default;
        OpenTypedRelic(OpenTypedRelic&& arg) noexcept = default;

        OpenTypedRelic& operator=(const OpenTypedRelic& arg) = default;
        OpenTypedRelic& operator=(OpenTypedRelic&& arg) noexcept = default;
    private:
        RelicID id;
        Reliquary* owner;
    private:
        template<class, class>
        friend class BatchSource;
    };
}