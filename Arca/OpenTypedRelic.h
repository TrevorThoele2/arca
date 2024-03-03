#pragma once

#include "RelicInit.h"
#include "Reliquary.h"
#include "TypeFor.h"

namespace Arca
{
    template<class Derived>
    class OpenTypedRelic
    {
    public:
        using Init = RelicInit;
    public:
        explicit OpenTypedRelic(Init init) :
            id(init.id), owner(&init.owner)
        {}

        OpenTypedRelic(const OpenTypedRelic& arg) = default;
        OpenTypedRelic(OpenTypedRelic && arg) noexcept = default;

        OpenTypedRelic& operator=(const OpenTypedRelic & arg) = default;
        OpenTypedRelic& operator=(OpenTypedRelic && arg) noexcept = default;

        template<class ShardT, class... ConstructorArgs, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        Index<ShardT> Create(ConstructorArgs&& ... constructorArgs) const
        {
            return owner->Do<Arca::Create<ShardT>>(id, std::forward<ConstructorArgs>(constructorArgs)...);
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
        [[nodiscard]] Index<ShardT> Find() const
        {
            return Arca::Index<ShardT>(id, *owner);
        }

        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        [[nodiscard]] Index<MatrixT> Find() const
        {
            return Arca::Index<MatrixT>(id, *owner);
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
    private:
        RelicID id;
        Reliquary* owner;
    private:
        template<class, class>
        friend class BatchSource;
    };
}