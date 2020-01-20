#pragma once

#include "ClosedRelic.h"
#include "Reliquary.h"

namespace Arca
{
    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    ShardIndex<ShardT> ClosedRelic::Find() const
    {
        return Arca::ShardIndex<ShardT>(id, Owner());
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    MatrixIndex<MatrixT> ClosedRelic::Find() const
    {
        return Arca::MatrixIndex<MatrixT>(id, Owner());
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    bool ClosedRelic::Contains() const
    {
        return static_cast<bool>(Find<ShardT>());
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    bool ClosedRelic::Contains() const
    {
        return owner->Contains<MatrixT>(id);
    }
}