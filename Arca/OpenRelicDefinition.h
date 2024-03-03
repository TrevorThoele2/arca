#pragma once

#include "OpenRelic.h"
#include "Reliquary.h"
#include "AsHandle.h"

namespace Arca
{
    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardIndex<ShardT> OpenRelic::Create()
    {
        return owner->shards.Create<ShardT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    void OpenRelic::Destroy()
    {
        owner->shards.Destroy<ShardT>(id);
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    void OpenRelic::Destroy()
    {
        owner->matrices.Destroy<MatrixT>(id);
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardIndex<ShardT> OpenRelic::Find() const
    {
        return Arca::ShardIndex<ShardT>(id, Owner());
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    MatrixIndex<MatrixT> OpenRelic::Find() const
    {
        return Arca::MatrixIndex<MatrixT>(id, Owner());
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    bool OpenRelic::Contains() const
    {
        return static_cast<bool>(Find<ShardT>());
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    bool OpenRelic::Contains() const
    {
        return owner->Contains<MatrixT>(id);
    }
}