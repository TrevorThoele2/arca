#pragma once

#include "RelicBatchSource.h"
#include "Reliquary.h"

namespace Arca
{
    template<class T>
    BatchSource<T, std::enable_if_t<is_relic_v<T>>>::BatchSource(Reliquary& owner) : owner(&owner)
    {}

    template<class T>
    template<class... ConstructorArgs>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Create(
        RelicID id, Reliquary& owner, ConstructorArgs&& ... constructorArgs)

        -> RelicT*
    {
        auto found = Find(id);
        if (found)
            return nullptr;

        return CreateImpl<T>(RelicInit{ id, owner }, std::forward<ConstructorArgs>(constructorArgs)...);
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Destroy(RelicID destroy) -> iterator
    {
        auto found = map.find(destroy);
        if (found != map.end())
            return map.erase(found);

        return map.end();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Destroy(iterator destroy) -> iterator
    {
        return map.erase(destroy);
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Destroy(const_iterator destroy) -> iterator
    {
        return map.erase(destroy);
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
        map.clear();
    }

    template<class T>
    void* BatchSource<T, std::enable_if_t<is_relic_v<T>>>::FindStorage(RelicID id)
    {
        auto found = map.find(id);
        if (found == map.end())
            return {};

        return &found->second;
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Find(RelicID id) -> RelicT*
    {
        return static_cast<RelicT*>(FindStorage(id));
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Size() const -> SizeT
    {
        return map.size();
    }

    template<class T>
    bool BatchSource<T, std::enable_if_t<is_relic_v<T>>>::IsEmpty() const
    {
        return map.empty();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::begin() -> iterator
    {
        return map.begin();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::begin() const -> const_iterator
    {
        return map.begin();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::end() -> iterator
    {
        return map.end();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::end() const -> const_iterator
    {
        return map.end();
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
        if (archive.IsOutput())
        {
            auto size = object.map.size();
            archive(size);

            for (auto& entry : object.map)
            {
                auto id = entry.first;
                archive(id);
                archive(entry.second);
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
                    auto& emplaced = object.map.emplace(id, std::move(relic)).first->second;
                    archive.types.AttemptReplaceTrackedObject(relic, emplaced);
                }
            }
        }
    }

    template<class T>
    void Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>>::
        Scriven(const std::string& name, ObjectT& object, JsonArchive& archive)
    {
        if (archive.IsOutput())
        {
            auto output = archive.AsOutput();
            output->StartList(name);
            for (auto& entry : object.map)
            {
                output->StartObject("");
                auto id = entry.first;
                archive("id", id);
                archive("relic", entry.second);
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
                auto& emplaced = object.map.emplace(id, std::move(relic)).first->second;
                archive.types.AttemptReplaceTrackedObject(relic, emplaced);

                input->EndObject();
            }
            input->EndList();
        }
    }

    template<class T>
    template<
        class U,
        std::enable_if_t<
            Arca::has_relic_init_serialization_constructor_v<U>, int>>
    U Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>>::Create(Arca::RelicInit init)
    {
        return ObjectT::RelicT{ init, Arca::Serialization{} };
    }

    template<class T>
    template<
        class U,
        std::enable_if_t<
            !Arca::has_relic_init_serialization_constructor_v<U> &&
            Arca::has_relic_serialization_constructor_v<U>, int>>
    U Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>>::Create(Arca::RelicInit init)
    {
        return ObjectT::RelicT{ Arca::Serialization{} };
    }

    template<class T>
    template<
        class U,
        std::enable_if_t<
            !Arca::has_relic_init_serialization_constructor_v<U> &&
            !Arca::has_relic_serialization_constructor_v<U> &&
            Arca::has_relic_init_constructor_v<U>, int>>
    U Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>>::Create(Arca::RelicInit init)
    {
        return ObjectT::RelicT{ init };
    }

    template<class T>
    template<
        class U,
        std::enable_if_t<
            !Arca::has_relic_init_serialization_constructor_v<U> &&
            !Arca::has_relic_serialization_constructor_v<U> &&
            !Arca::has_relic_init_constructor_v<U> &&
            Chroma::is_braces_default_constructible_v<U>, int>>
    U Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>>::Create(Arca::RelicInit init)
    {
        return ObjectT::RelicT{};
    }

    template<class T>
    template<
        class U,
        std::enable_if_t<
            !Arca::has_relic_init_serialization_constructor_v<U> &&
            !Arca::has_relic_serialization_constructor_v<U> &&
            !Arca::has_relic_init_constructor_v<U> &&
            !Chroma::is_braces_default_constructible_v<U>, int>>
    U Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>>::Create(Arca::RelicInit init)
    {
        static_assert(
            false,
            "A relic requires a constructor of form "
            "(RelicInit, Serialization), "
            "(Serialization), "
            "(RelicInit) or"
            "()"
            "in order to be serialized. Order given is priority order.");
        return ObjectT::RelicT{ init };
    }
}