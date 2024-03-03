#pragma once

#include "RelicBatchSource.h"
#include "Reliquary.h"

namespace Arca
{
    template<class T>
    BatchSource<T, std::enable_if_t<is_relic_v<T>>>::BatchSource(Reliquary& owner) : owner(&owner)
    {}

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Add(RelicT&& relic) -> RelicT*
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
        for (auto loop = list.begin(); loop != list.end(); ++loop)
            if (loop->ID() == destroy)
                return list.erase(loop);

        return list.end();
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
    void BatchSource<T, std::enable_if_t<is_relic_v<T>>>::DestroyAllFromBase(Reliquary& reliquary)
    {
        reliquary.relics.Clear<T>();
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Clear()
    {
        list.clear();
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_relic_v<T>>>::SetOwner(Reliquary& owner)
    {
        for (auto& loop : list)
            loop.owner = &owner;
    }

    template<class T>
    void* BatchSource<T, std::enable_if_t<is_relic_v<T>>>::FindStorage(RelicID id)
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
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Find(RelicID id) -> RelicT*
    {
        return static_cast<RelicT*>(FindStorage(id));
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

    template<class T>
    Arca::Type BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Type() const
    {
        return TypeFor<T>();
    }
}

namespace Inscription
{
    template<class T>
    void Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>>::
        Scriven(ObjectT& object, BinaryArchive& archive)
    {
        DoScriven<typename ObjectT::RelicT>(object, archive);
    }

    template<class T>
    void Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>>::
        Scriven(const std::string& name, ObjectT& object, JsonArchive& archive)
    {
        DoScriven<typename ObjectT::RelicT>(name, object, archive);
    }

    template<class T>
    template<class U, std::enable_if_t<Arca::HasScribe<U, BinaryArchive>(), int>>
    void Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>>::
        DoScriven(ObjectT& object, BinaryArchive& archive)
    {
        if (archive.IsOutput())
        {
            auto size = object.list.size();
            archive(size);

            for (auto& relic : object.list)
            {
                auto id = relic.ID();
                archive(id);
                archive(relic);
            }
        }
        else
        {
            ContainerSize size;
            archive(size);

            while (size-- > 0)
            {
                Arca::RelicID id = 0;
                archive(id);

                auto foundRelic = object.Find(id);
                if (foundRelic)
                    archive(*foundRelic);
                else
                {
                    auto relic = Create<RelicT>(Arca::RelicInit{ id, *object.owner });
                    archive(relic);
                    object.list.push_back(std::move(relic));
                    archive.types.AttemptReplaceTrackedObject(relic, object.list.back());
                }
            }
        }
    }

    template<class T>
    template<class U, std::enable_if_t<!Arca::HasScribe<U, BinaryArchive>(), int>>
    void Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>>::
        DoScriven(ObjectT&, BinaryArchive&)
    {}

    template<class T>
    template<class U, std::enable_if_t<Arca::HasScribe<U, JsonArchive>(), int>>
    void Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>>::
        DoScriven(const std::string& name, ObjectT& object, JsonArchive& archive)
    {
        if (archive.IsOutput())
        {
            auto output = archive.AsOutput();
            output->StartList(name);
            for (auto& relic : object.list)
            {
                output->StartObject("");
                auto id = relic.ID();
                archive("id", id);
                archive("relic", relic);
                output->EndObject();
            }
            output->EndList();
        }
        else
        {
            auto input = archive.AsInput();
            auto size = input->StartList(name);
            while (size-- > 0)
            {
                input->StartObject("");

                Arca::RelicID id = 0;
                archive("id", id);

                auto relic = Create<RelicT>(Arca::RelicInit{ id, *object.owner });
                archive("relic", relic);
                object.list.push_back(std::move(relic));
                archive.types.AttemptReplaceTrackedObject(relic, object.list.back());

                input->EndObject();
            }
            input->EndList();
        }
    }

    template<class T>
    template<class U, std::enable_if_t<!Arca::HasScribe<U, JsonArchive>(), int>>
    void Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>>::
        DoScriven(const std::string& name, ObjectT&, JsonArchive&)
    {}

    template<class T>
    template<class U, std::enable_if_t<Arca::has_relic_serialization_constructor_v<U>, int>>
    U Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>>::Create(Arca::RelicInit init)
    {
        return ObjectT::RelicT{ init, Arca::Serialization{} };
    }

    template<class T>
    template<class U, std::enable_if_t<!Arca::has_relic_serialization_constructor_v<U> && Arca::has_relic_default_constructor_v<U>, int>>
    U Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>>::Create(Arca::RelicInit init)
    {
        return ObjectT::RelicT{ init };
    }

    template<class T>
    template<class U, std::enable_if_t<!Arca::has_relic_serialization_constructor_v<U> && !Arca::has_relic_default_constructor_v<U>, int>>
    U Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>>::Create(Arca::RelicInit init)
    {
        static_assert(
            false,
            "A relic requires a serialization constructor (taking a RelicInit and Serialization) "
            "or a constructor taking only RelicInit in order to be serialized.");
        return ObjectT::RelicT{ init };
    }
}