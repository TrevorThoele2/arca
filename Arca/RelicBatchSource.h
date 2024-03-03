#pragma once

#include "VesselID.h"

#include "Serialization.h"

namespace Arca
{
    class RelicBatchSourceBase
    {
    public:
        using SizeT = size_t;
    public:
        virtual ~RelicBatchSourceBase() = 0;

        virtual void DestroyFromBase(VesselID id) = 0;

        [[nodiscard]] virtual SizeT Size() const = 0;
    protected:
        friend class Reliquary;
    };

    template<class T>
    class RelicBatchSource : public RelicBatchSourceBase
    {
    public:
        using RelicT = T;
    private:
        struct Entry
        {
            VesselID id;
            RelicT relic;
        };

        using List = std::vector<Entry>;
    public:
        using iterator = typename List::iterator;
        using const_iterator = typename List::const_iterator;
    public:
        RelicBatchSource() = default;

        RelicT* Add(VesselID id);

        iterator Destroy(VesselID destroy);
        iterator Destroy(iterator destroy);
        iterator Destroy(const_iterator destroy);

        void DestroyFromBase(VesselID id) override;

        RelicT* Find(VesselID id);

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
    auto RelicBatchSource<T>::Add(VesselID id) -> RelicT*
    {
        auto found = Find(id);
        if (found)
            return found;

        list.push_back({ id, RelicT{} });
        return &list.back().relic;
    }

    template<class T>
    auto RelicBatchSource<T>::Destroy(VesselID destroy) -> iterator
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
    auto RelicBatchSource<T>::Destroy(iterator destroy) -> iterator
    {
        return list.erase(destroy);
    }

    template<class T>
    auto RelicBatchSource<T>::Destroy(const_iterator destroy) -> iterator
    {
        return list.erase(destroy);
    }

    template<class T>
    void RelicBatchSource<T>::DestroyFromBase(VesselID id)
    {
        Destroy(id);
    }

    template<class T>
    auto RelicBatchSource<T>::Find(VesselID id) -> RelicT*
    {
        auto found = std::find_if(
            list.begin(),
            list.end(),
            [id](const Entry& entry) { return entry.id == id; });
        if (found == list.end())
            return {};

        return &found->relic;
    }

    template<class T>
    auto RelicBatchSource<T>::Size() const -> SizeT
    {
        return list.size();
    }

    template<class T>
    bool RelicBatchSource<T>::IsEmpty() const
    {
        return list.empty();
    }

    template<class T>
    auto RelicBatchSource<T>::begin() -> iterator
    {
        return list.begin();
    }

    template<class T>
    auto RelicBatchSource<T>::begin() const -> const_iterator
    {
        return list.begin();
    }

    template<class T>
    auto RelicBatchSource<T>::end() -> iterator
    {
        return list.end();
    }

    template<class T>
    auto RelicBatchSource<T>::end() const -> const_iterator
    {
        return list.end();
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<::Arca::RelicBatchSource<T>, BinaryArchive> final :
        public CompositeScribe<::Arca::RelicBatchSource<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<::Arca::RelicBatchSource<T>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;

        using BaseT::Scriven;
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };

    template<class T>
    void Scribe<::Arca::RelicBatchSource<T>, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        if (archive.IsOutput())
        {
            auto size = object.list.size();
            archive(size);

            for(auto& loop : object.list)
            {
                auto id = loop.id;
                archive(id);
                auto relic = loop.relic;
                archive(relic);
            }
        }
        else
        {
            ContainerSize size;
            archive(size);

            object.list.clear();

            while(size-- > 0)
            {
                ::Arca::VesselID id;
                archive(id);

                typename ObjectT::RelicT relic;
                archive(relic);

                object.list.push_back({ id, std::move(relic) });
            }
        }
    }
}