#pragma once

#include "OpenTypedRelic.h"
#include "Reliquary.h"

namespace Arca
{
    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardIndex<ShardT> OpenTypedRelic::Create()
    {
        return owner->shards.Create<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    void OpenTypedRelic::Destroy()
    {
        owner->shards.Destroy<ShardT>(id);
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    void OpenTypedRelic::Destroy()
    {
        owner->shards.Destroy<MatrixT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardIndex<ShardT> OpenTypedRelic::Find() const
    {
        return Arca::ShardIndex<ShardT>(id, *owner);
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    MatrixIndex<MatrixT> OpenTypedRelic::Find() const
    {
        return Arca::MatrixIndex<MatrixT>(id, *owner);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    bool OpenTypedRelic::Contains() const
    {
        return static_cast<bool>(Find<ShardT>());
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    bool OpenTypedRelic::Contains() const
    {
        return owner->Contains<MatrixT>(id);
    }
}