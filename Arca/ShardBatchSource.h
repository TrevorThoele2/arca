#pragma once

#include <vector>

#include "BatchSource.h"
#include "RelicID.h"
#include "ShardTraits.h"

#include "Serialization.h"

namespace Arca
{
    class ShardBatchSourceBase
    {
    public:
        using SizeT = size_t;
    public:
        virtual ~ShardBatchSourceBase() = 0;

        virtual void DestroyFromBase(RelicID id) = 0;

        [[nodiscard]] virtual SizeT Size() const = 0;
    protected:
        friend class Reliquary;
    };

    template<class T>
    class BatchSource<T, std::enable_if_t<is_shard_v<T>>> : public ShardBatchSourceBase
    {
    public:
        using ShardT = T;
    private:
        using StoredT = std::decay_t<ShardT>;

        struct Entry
        {
            RelicID id;
            StoredT shard;

            Entry(RelicID id, StoredT&& shard);
        };

        using List = std::vector<Entry>;
    public:
        using iterator = typename List::iterator;
        using const_iterator = typename List::const_iterator;
    public:
        BatchSource() = default;

        ShardT* Add(RelicID id);

        iterator Destroy(RelicID destroy);
        iterator Destroy(iterator destroy);
        iterator Destroy(const_iterator destroy);

        void DestroyFromBase(RelicID id) override;

        ShardT* Find(RelicID id);

        [[nodiscard]] SizeT Size() const override;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;
    private:
        List list;
    private:
        friend class Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Entry::Entry(RelicID id, StoredT&& shard)
        : id(id), shard(std::move(shard))
    {}

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Add(RelicID id) -> ShardT*
    {
        auto found = Find(id);
        if (found)
            return found;

        list.emplace_back(id, StoredT{});
        return &list.back().shard;
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Destroy(RelicID destroy) -> iterator
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
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Destroy(iterator destroy) -> iterator
    {
        return list.erase(destroy);
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Destroy(const_iterator destroy) -> iterator
    {
        return list.erase(destroy);
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_shard_v<T>>>::DestroyFromBase(RelicID id)
    {
        Destroy(id);
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Find(RelicID id) -> ShardT*
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

        return &found->shard;
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Size() const -> SizeT
    {
        return list.size();
    }

    template<class T>
    bool BatchSource<T, std::enable_if_t<is_shard_v<T>>>::IsEmpty() const
    {
        return list.empty();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::begin() -> iterator
    {
        return list.begin();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::begin() const -> const_iterator
    {
        return list.begin();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::end() -> iterator
    {
        return list.end();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::end() const -> const_iterator
    {
        return list.end();
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<::Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>, BinaryArchive> final :
        public CompositeScribe<::Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<::Arca::BatchSource<T>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;

        using BaseT::Scriven;
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };

    template<class T>
    void Scribe<::Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>, BinaryArchive>::
        ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        if (archive.IsOutput())
        {
            auto size = object.list.size();
            archive(size);

            for(auto& loop : object.list)
            {
                auto id = loop.id;
                archive(id);
                auto shard = loop.shard;
                archive(shard);
            }
        }
        else
        {
            ContainerSize size;
            archive(size);

            object.list.clear();

            while(size-- > 0)
            {
                ::Arca::RelicID id;
                archive(id);

                typename ObjectT::StoredT shard;
                archive(shard);

                object.list.emplace_back(id, std::move(shard));
            }
        }
    }
}