#pragma once

#include "OpenRelic.h"
#include "Reliquary.h"
#include "AsHandle.h"

namespace Arca
{
    template<class ShardT, class... ConstructorArgs, std::enable_if_t<is_shard_v<ShardT>, int>>
    Index<ShardT> OpenRelic::Create(ConstructorArgs&& ... constructorArgs) const
    {
        return owner->Do(Arca::Create<ShardT>{id, std::forward<ConstructorArgs>(constructorArgs)...});
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    void OpenRelic::Destroy() const
    {
        owner->Do(Arca::Destroy<ShardT>{id});
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    void OpenRelic::Destroy() const
    {
        owner->Do(Arca::Destroy<MatrixT>{id});
    }

    template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int>>
    Index<ShardT> OpenRelic::Find() const
    {
        return Arca::Index<ShardT>(id, Owner());
    }

    template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int>>
    Index<MatrixT> OpenRelic::Find() const
    {
        return Arca::Index<MatrixT>(id, Owner());
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