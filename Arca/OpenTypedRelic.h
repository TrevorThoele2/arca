#pragma once

#include "RelicID.h"

#include "IsShard.h"
#include "IsMatrix.h"

#include "ShardIndex.h"
#include "MatrixIndex.h"
#include "Handle.h"

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
        ShardIndex<ShardT> Create();
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        void Destroy();
        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        void Destroy();
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardIndex<ShardT> Find() const;
        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        [[nodiscard]] MatrixIndex<MatrixT> Find() const;
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] bool Contains() const;
        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        [[nodiscard]] bool Contains() const;

        [[nodiscard]] std::optional<Handle> Parent() const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Reliquary& Owner() const;
    public:
        virtual ~OpenTypedRelic() = 0;
    protected:
        OpenTypedRelic() = default;

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