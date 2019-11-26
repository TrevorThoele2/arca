#pragma once

#include <list>

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

        [[nodiscard]] virtual SizeT TotalSize() const = 0;
        [[nodiscard]] virtual SizeT NonConstSize() const = 0;
    protected:
        friend class Reliquary;
    };

    template<class T>
    class BatchSource<T, std::enable_if_t<is_shard_v<T>>> : public ShardBatchSourceBase
    {
    public:
        using ShardT = T;
    private:
        struct Entry
        {
            RelicID id;
            ShardT shard;
            bool isConst;
        };

        using List = std::list<Entry>;
    public:
        using iterator = typename List::iterator;
        using const_iterator = typename List::const_iterator;
    public:
        BatchSource() = default;

        ShardT* Add(RelicID id, bool isConst);

        iterator Destroy(RelicID destroy);
        iterator Destroy(iterator destroy);
        iterator Destroy(const_iterator destroy);

        void DestroyFromBase(RelicID id) override;

        ShardT* Find(RelicID id, bool isConst);

        [[nodiscard]] SizeT TotalSize() const override;
        [[nodiscard]] SizeT NonConstSize() const override;
        [[nodiscard]] bool IsEmpty() const;
        [[nodiscard]] bool IsNonConstEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;
    private:
        List list;
        size_t nonConstSize = 0;
    private:
        friend class Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Add(RelicID id, bool isConst) -> ShardT*
    {
        auto found = Find(id, isConst);
        if (found)
            return found;

        list.push_back({ id, ShardT{}, isConst });
        auto shard = &list.back().shard;

        if (!isConst)
            ++nonConstSize;

        return shard;
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

        if (!itr->isConst)
            --nonConstSize;

        return list.erase(itr);
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Destroy(iterator destroy) -> iterator
    {
        if (!destroy->isConst)
            --nonConstSize;
        return list.erase(destroy);
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Destroy(const_iterator destroy) -> iterator
    {
        if (!destroy->isConst)
            --nonConstSize;
        return list.erase(destroy);
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_shard_v<T>>>::DestroyFromBase(RelicID id)
    {
        Destroy(id);
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Find(RelicID id, bool isConst) -> ShardT*
    {
        auto found = std::find_if(
            list.begin(),
            list.end(),
            [id, isConst](const Entry& entry)
            {
                if (entry.id != id)
                    return false;

                return entry.isConst ? isConst : true;
            });
        if (found == list.end())
            return {};

        return &found->shard;
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::TotalSize() const -> SizeT
    {
        return list.size();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::NonConstSize() const -> SizeT
    {
        return nonConstSize;
    }

    template<class T>
    bool BatchSource<T, std::enable_if_t<is_shard_v<T>>>::IsEmpty() const
    {
        return list.empty();
    }

    template<class T>
    bool BatchSource<T, std::enable_if_t<is_shard_v<T>>>::IsNonConstEmpty() const
    {
        return nonConstSize == 0;
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
                auto isConst = loop.isConst;
                archive(isConst);
            }
        }
        else
        {
            ContainerSize size;
            archive(size);

            object.list.clear();

            size_t nonConstSize = 0;

            while(size-- > 0)
            {
                ::Arca::RelicID id;
                archive(id);

                typename ObjectT::ShardT shard;
                archive(shard);

                auto isConst = false;
                archive(isConst);

                object.list.push_back({ id, std::move(shard), isConst });

                if (!isConst)
                    ++nonConstSize;
            }

            object.nonConstSize = nonConstSize;
        }
    }
}