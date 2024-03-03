#pragma once

#include <list>

#include "RelicID.h"

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
    class ShardBatchSource : public ShardBatchSourceBase
    {
    public:
        using ShardT = T;
    private:
        struct Entry
        {
            RelicID id;
            ShardT shard;
        };

        using List = std::list<Entry>;
    public:
        using iterator = typename List::iterator;
        using const_iterator = typename List::const_iterator;
    public:
        ShardBatchSource() = default;

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
    auto ShardBatchSource<T>::Add(RelicID id) -> ShardT*
    {
        auto found = Find(id);
        if (found)
            return found;

        list.push_back({ id, ShardT{} });
        return &list.back().shard;
    }

    template<class T>
    auto ShardBatchSource<T>::Destroy(RelicID destroy) -> iterator
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
    auto ShardBatchSource<T>::Destroy(iterator destroy) -> iterator
    {
        return list.erase(destroy);
    }

    template<class T>
    auto ShardBatchSource<T>::Destroy(const_iterator destroy) -> iterator
    {
        return list.erase(destroy);
    }

    template<class T>
    void ShardBatchSource<T>::DestroyFromBase(RelicID id)
    {
        Destroy(id);
    }

    template<class T>
    auto ShardBatchSource<T>::Find(RelicID id) -> ShardT*
    {
        auto found = std::find_if(
            list.begin(),
            list.end(),
            [id](const Entry& entry) { return entry.id == id; });
        if (found == list.end())
            return {};

        return &found->shard;
    }

    template<class T>
    auto ShardBatchSource<T>::Size() const -> SizeT
    {
        return list.size();
    }

    template<class T>
    bool ShardBatchSource<T>::IsEmpty() const
    {
        return list.empty();
    }

    template<class T>
    auto ShardBatchSource<T>::begin() -> iterator
    {
        return list.begin();
    }

    template<class T>
    auto ShardBatchSource<T>::begin() const -> const_iterator
    {
        return list.begin();
    }

    template<class T>
    auto ShardBatchSource<T>::end() -> iterator
    {
        return list.end();
    }

    template<class T>
    auto ShardBatchSource<T>::end() const -> const_iterator
    {
        return list.end();
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<::Arca::ShardBatchSource<T>, BinaryArchive> final :
        public CompositeScribe<::Arca::ShardBatchSource<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<::Arca::ShardBatchSource<T>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;

        using BaseT::Scriven;
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };

    template<class T>
    void Scribe<::Arca::ShardBatchSource<T>, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
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

                typename ObjectT::ShardT shard;
                archive(shard);

                object.list.push_back({ id, std::move(shard) });
            }
        }
    }
}