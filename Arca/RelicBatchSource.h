#pragma once

#include <vector>

#include "BatchSource.h"
#include "RelicID.h"
#include "RelicTraits.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    class RelicBatchSourceBase
    {
    public:
        using SizeT = size_t;
    public:
        virtual ~RelicBatchSourceBase() = 0;

        virtual void DestroyFromBase(RelicID id) = 0;

        [[nodiscard]] virtual SizeT Size() const = 0;
    };

    template<class T>
    class BatchSource<T, std::enable_if_t<is_relic_v<T>>> : public RelicBatchSourceBase
    {
    public:
        using RelicT = T;
    private:
        using List = std::vector<RelicT>;
    public:
        using iterator = typename List::iterator;
        using const_iterator = typename List::const_iterator;
    public:
        explicit BatchSource(Reliquary& owner);

        RelicT* Add(RelicT relic);

        iterator Destroy(RelicID destroy);
        iterator Destroy(iterator destroy);
        iterator Destroy(const_iterator destroy);

        void DestroyFromBase(RelicID id) override;

        RelicT* Find(RelicID id);

        [[nodiscard]] SizeT Size() const override;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;
    private:
        List list;
        Reliquary* owner;
    private:
        friend class Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    BatchSource<T, std::enable_if_t<is_relic_v<T>>>::BatchSource(Reliquary& owner) : owner(&owner)
    {}

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Add(RelicT relic) -> RelicT*
    {
        auto found = Find(relic.ID());
        if (found)
            return nullptr;

        list.push_back(std::move(relic));
        return &list.back();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Destroy(RelicID destroy) -> iterator
    {
        auto itr = std::remove_if(
            list.begin(),
            list.end(),
            [destroy](const RelicT& entry) { return entry.ID() == destroy; });
        if (itr == list.end())
            return list.end();
        return list.erase(itr);
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Destroy(iterator destroy) -> iterator
    {
        return list.erase(destroy);
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Destroy(const_iterator destroy) -> iterator
    {
        return list.erase(destroy);
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_relic_v<T>>>::DestroyFromBase(RelicID id)
    {
        Destroy(id);
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Find(RelicID id) -> RelicT*
    {
        auto found = std::find_if(
            list.begin(),
            list.end(),
            [id](const RelicT& entry) { return entry.ID() == id; });
        if (found == list.end())
            return {};

        return &*found;
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Size() const -> SizeT
    {
        return list.size();
    }

    template<class T>
    bool BatchSource<T, std::enable_if_t<is_relic_v<T>>>::IsEmpty() const
    {
        return list.empty();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::begin() -> iterator
    {
        return list.begin();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::begin() const -> const_iterator
    {
        return list.begin();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::end() -> iterator
    {
        return list.end();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::end() const -> const_iterator
    {
        return list.end();
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<::Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>, BinaryArchive> final :
        public CompositeScribe<::Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>, BinaryArchive>
    {
    private:
        using BaseT =
            CompositeScribe<::Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;

        using BaseT::Scriven;
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };

    template<class T>
    void Scribe<::Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>, BinaryArchive>::
        ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        if (archive.IsOutput())
        {
            auto size = object.list.size();
            archive(size);

            for (auto& loop : object.list)
            {
                auto id = loop.ID();
                archive(id);
                archive(loop);
            }
        }
        else
        {
            ContainerSize size;
            archive(size);

            object.list.clear();

            while (size-- > 0)
            {
                Arca::RelicID id = 0;
                archive(id);

                ScopeConstructor<typename ObjectT::RelicT> constructor(archive);
                object.list.push_back(std::move(constructor.GetMove()));
                archive.AttemptReplaceTrackedObject(*constructor.Get(), object.list.back());

                object.list.back().id = id;
                object.list.back().owner = object.owner;
            }
        }
    }
}