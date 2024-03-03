#pragma once

#include "ShardBatchSource.h"
#include "Reliquary.h"

namespace Arca
{
    template<class T>
    BatchSource<T, std::enable_if_t<is_shard_v<T>>>::BatchSource(Reliquary& owner) : owner(&owner)
    {}

    template<class T>
    template<class... ConstructorArgs>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Add(RelicID id, bool required, ConstructorArgs&& ... constructorArgs) -> ShardT*
    {
        auto found = Find(id);
        if (found)
            return found;

        return &map.emplace(
            id,
            StoredT{ required, std::decay_t<ShardT>{std::forward<ConstructorArgs>(constructorArgs)...} })
            .first->second.shard;
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Destroy(RelicID destroy) -> iterator
    {
        auto found = map.find(destroy);
        if (found != map.end())
            return map.erase(found);

        return map.end();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Destroy(iterator destroy) -> iterator
    {
        return map.erase(destroy);
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Destroy(const_iterator destroy) -> iterator
    {
        return map.erase(destroy);
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
        map.clear();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Find(RelicID id) -> ShardT*
    {
        auto found = map.find(id);
        return found != map.end() ? &found->second.shard : nullptr;
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
    bool BatchSource<T, std::enable_if_t<is_shard_v<T>>>::IsRequired(RelicID id) const
    {
        auto found = map.find(id);
        return found != map.end() ? found->second.required : false;
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Size() const -> SizeT
    {
        return map.size();
    }

    template<class T>
    bool BatchSource<T, std::enable_if_t<is_shard_v<T>>>::IsEmpty() const
    {
        return map.empty();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::begin() -> iterator
    {
        return map.begin();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::begin() const -> const_iterator
    {
        return map.begin();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::end() -> iterator
    {
        return map.end();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::end() const -> const_iterator
    {
        return map.end();
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
    void Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>>::
        Scriven(ObjectT& object, Archive::Binary& archive)
    {
        if (archive.IsOutput())
        {
            auto& saveIDs = archive.UserContext<SaveUserContext>()->ids;

            std::vector<typename ObjectT::iterator> save;
            for (auto& id : saveIDs)
            {
                auto found = object.map.find(id);
                if (found != object.map.end())
                    save.push_back(found);
            }

            auto size = save.size();
            archive(size);

            for (auto& stored : save)
            {
                if (saveIDs.find(stored->first) != saveIDs.end())
                {
                    archive(stored->first);
                    archive(stored->second.shard);
                    archive(stored->second.required);
                }
            }
        }
        else
        {
            ContainerSize size;
            archive(size);

            while (size-- > 0)
            {
                Arca::RelicID id;
                archive(id);

                auto matrixSnapshot = object.owner->matrices.StartCreationTransaction(id);

                auto foundShard = object.Find(id);
                if (foundShard)
                {
                    archive(*foundShard);
                    bool required = false;
                    archive(required);
                }
                else
                {
                    auto shard = Create<ShardT>();
                    archive(shard);
                    bool required = false;
                    archive(required);
                    object.map.emplace(id, typename ObjectT::StoredT{ required, std::move(shard) });
                }

                matrixSnapshot.Finalize();
            }
        }
    }

    template<class T>
    void Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>>::
        Scriven(const std::string& name, ObjectT& object, Archive::Json& archive)
    {
        if (archive.IsOutput())
        {
            auto& saveIDs = archive.UserContext<SaveUserContext>()->ids;

            std::vector<typename ObjectT::iterator> save;
            for(auto& id : saveIDs)
            {
                auto found = object.map.find(id);
                if (found != object.map.end())
                    save.push_back(found);
            }

            auto output = archive.AsOutput();
            output->StartList(name);
            for (auto& stored : save)
            {
                output->StartObject("");
                archive("id", stored->first);
                archive("shard", stored->second.shard);
                archive("required", stored->second.required);
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

                Arca::RelicID id = Arca::nullRelicID;
                archive("id", id);

                auto matrixSnapshot = object.owner->matrices.StartCreationTransaction(id);

                auto foundShard = object.Find(id);
                if (foundShard)
                {
                    archive("shard", *foundShard);
                    bool required = false;
                    archive("required", required);
                }
                else
                {
                    auto shard = Create<ShardT>();
                    archive("shard", shard);
                    bool required = false;
                    archive("required", required);
                    object.map.emplace(id, typename ObjectT::StoredT{ required, std::move(shard) });
                }

                matrixSnapshot.Finalize();

                input->EndObject();
            }
            input->EndList();
        }
    }

    template<class T>
    template<class U, std::enable_if_t<Arca::has_shard_serialization_constructor_v<U>, int>>
    U Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>>::Create()
    {
        return std::decay_t<typename ObjectT::ShardT>{ Arca::Serialization{} };
    }

    template<class T>
    template<class U, std::enable_if_t<!Arca::has_shard_serialization_constructor_v<U> && Arca::has_shard_default_constructor_v<U>, int>>
    U Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>>::Create()
    {
        return std::decay_t<typename ObjectT::ShardT>{};
    }

    template<class T>
    template<class U, std::enable_if_t<!Arca::has_shard_serialization_constructor_v<U> && !Arca::has_shard_default_constructor_v<U>, int>>
    U Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>>::Create()
    {
        static_assert(
            "A shard requires a serialization constructor (taking only the class Serialization) "
            "or a default constructor in order to be serialized.");
        return std::decay_t<typename ObjectT::ShardT>{};
    }
}