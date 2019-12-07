#pragma once

#include "EitherShardBatchSource.h"
#include "ReliquaryShards.h"

namespace Arca
{
    template<class T>
    BatchSource<T, std::enable_if_t<is_either_v<T>>>::Entry::Entry(RelicID id, const ShardT& shard, bool isConst)
        : id(id), shard(&shard), isConst(isConst)
    {}

    template<class T>
    BatchSource<T, std::enable_if_t<is_either_v<T>>>::BatchSource(ReliquaryShards& owner) : owner(&owner)
    {}

    template<class T>
    void BatchSource<T, std::enable_if_t<is_either_v<T>>>::Add(RelicID id, const ShardT& add, bool isConst)
    {
        auto found = Find(id, isConst);
        if (found)
            return;

        list.push_back(Entry { id, add, isConst });
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_either_v<T>>>::Destroy(RelicID destroy, bool isConst) -> iterator
    {
        auto itr = std::remove_if(
            list.begin(),
            list.end(),
            [destroy, isConst](const Entry& entry)
            {
                return entry.id == destroy && entry.isConst == isConst;
            });
        if (itr == list.end())
            return list.end();

        return list.erase(itr);
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_either_v<T>>>::Destroy(RelicID destroy) -> iterator
    {
        auto itr = std::remove_if(
            list.begin(),
            list.end(),
            [destroy](const Entry& entry) { return entry.id == destroy; });
        if (itr == list.end())
            return list.end();

        return list.erase(itr);
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_either_v<T>>>::Destroy(iterator destroy) -> iterator
    {
        return list.erase(destroy);
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_either_v<T>>>::Destroy(const_iterator destroy) -> iterator
    {
        return list.erase(destroy);
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_either_v<T>>>::DestroyFromBase(RelicID id, bool isConst)
    {
        Destroy(id);
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_either_v<T>>>::DestroyFromBase(RelicID id)
    {
        Destroy(id);
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_either_v<T>>>::Find(RelicID id, bool isConst) -> const ShardT*
    {
        auto found = std::find_if(
            list.begin(),
            list.end(),
            [id, isConst](const Entry& entry)
            {
                return entry.id == id && entry.isConst == isConst;
            });
        if (found == list.end())
            return {};

        return found->shard;
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_either_v<T>>>::Find(RelicID id) -> const ShardT*
    {
        auto found = std::find_if(
            list.begin(),
            list.end(),
            [id](const Entry& entry)
            {
                return entry.id == id;
            });
        if (found == list.end())
            return {};

        return found->shard;
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_either_v<T>>>::Size() const -> SizeT
    {
        return list.size();
    }

    template<class T>
    bool BatchSource<T, std::enable_if_t<is_either_v<T>>>::IsEmpty() const
    {
        return list.empty();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_either_v<T>>>::begin() -> iterator
    {
        return list.begin();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_either_v<T>>>::begin() const -> const_iterator
    {
        return list.begin();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_either_v<T>>>::end() -> iterator
    {
        return list.end();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_either_v<T>>>::end() const -> const_iterator
    {
        return list.end();
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_either_v<T>>>::Reference()
    {
        ++referenceCount;
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_either_v<T>>>::Dereference()
    {
        --referenceCount;
        if (referenceCount == 0)
            owner->eitherBatchSources.map.erase(TypeFor<ShardT>().name);
    }
}