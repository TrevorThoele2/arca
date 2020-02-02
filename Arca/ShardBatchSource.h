#pragma once

#include <vector>

#include "BatchSource.h"
#include "IsShard.h"

#include "RelicID.h"
#include "TypeFor.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;
    
    class ShardBatchSourceBase
    {
    public:
        using SizeT = size_t;
    public:
        virtual ~ShardBatchSourceBase() = 0;

        virtual bool DestroyFromBase(RelicID id) = 0;

        [[nodiscard]] virtual bool ContainsFromBase(RelicID id) const = 0;

        [[nodiscard]] virtual SizeT Size() const = 0;

        [[nodiscard]] virtual Type Type() const = 0;
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
        explicit BatchSource(Reliquary& owner);
        BatchSource(const BatchSource& arg) = delete;
        BatchSource(BatchSource&& arg) = default;

        template<class... ConstructorArgs>
        ShardT* Add(RelicID id, ConstructorArgs&& ... constructorArgs);

        iterator Destroy(RelicID destroy);
        iterator Destroy(iterator destroy);
        iterator Destroy(const_iterator destroy);

        bool DestroyFromBase(RelicID id) override;

        void Clear();

        [[nodiscard]] ShardT* Find(RelicID id);
        [[nodiscard]] const ShardT* Find(RelicID id) const;

        [[nodiscard]] bool ContainsFromBase(RelicID id) const override;

        [[nodiscard]] SizeT Size() const override;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;

        [[nodiscard]] Arca::Type Type() const override;
    private:
        List list;
    private:
        Reliquary* owner = nullptr;
        friend class Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Entry::Entry(RelicID id, StoredT&& shard)
        : id(id), shard(std::move(shard))
    {}

    template<class T>
    BatchSource<T, std::enable_if_t<is_shard_v<T>>>::BatchSource(Reliquary& owner) : owner(&owner)
    {}

    template<class T>
    template<class... ConstructorArgs>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Add(RelicID id, ConstructorArgs&& ... constructorArgs) -> ShardT*
    {
        auto found = Find(id);
        if (found)
            return found;

        list.emplace_back(id, StoredT{std::forward<ConstructorArgs>(constructorArgs)...});
        return &list.back().shard;
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Destroy(RelicID destroy) -> iterator
    {
        for (auto loop = list.begin(); loop != list.end(); ++loop)
            if (loop->id == destroy)
                return list.erase(loop);

        return list.end();
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
    bool BatchSource<T, std::enable_if_t<is_shard_v<T>>>::DestroyFromBase(RelicID id)
    {
        const auto size = Size();
        Destroy(id);
        return size != Size();
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Clear()
    {
        list.clear();
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
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Find(RelicID id) const -> const ShardT*
    {
        return const_cast<BatchSource&>(*this).Find(id);
    }

    template<class T>
    bool BatchSource<T, std::enable_if_t<is_shard_v<T>>>::ContainsFromBase(RelicID id) const
    {
        return Find(id) != nullptr;
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

    template<class T>
    Arca::Type BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Type() const
    {
        return TypeFor<T>();
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

            while(size-- > 0)
            {
                ::Arca::RelicID id;
                archive(id);

                auto foundShard = object.Find(id);
                if (foundShard)
                    archive(*foundShard);
                else
                {
                    typename ObjectT::StoredT shard;
                    archive(shard);
                    object.list.emplace_back(id, std::move(shard));
                    object.owner->matrices.NotifyCreated(id);
                }
            }
        }
    }
}