#pragma once

#include "ShardBatchSource.h"
#include "Reliquary.h"

namespace Arca
{
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

        list.emplace_back(id, StoredT{ std::forward<ConstructorArgs>(constructorArgs)... });
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
    void Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>>::
        Scriven(ObjectT& object, BinaryArchive& archive)
    {
        DoScriven<typename ObjectT::ShardT>(object, archive);
    }

    template<class T>
    void Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>>::
        Scriven(const std::string& name, ObjectT& object, JsonArchive& archive)
    {
        DoScriven<typename ObjectT::ShardT>(name, object, archive);
    }

    template<class T>
    template<class U, std::enable_if_t<Arca::HasScribe<U, BinaryArchive>(), int>>
    void Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>>::
        DoScriven(ObjectT& object, BinaryArchive& archive)
    {
        if (archive.IsOutput())
        {
            auto& saveIDs = archive.UserContext<SaveUserContext>()->ids;

            typename ObjectT::List save;
            std::copy_if(
                object.list.begin(),
                object.list.end(),
                std::back_inserter(save),
                [&saveIDs](const typename ObjectT::Entry& entry)
                {
                    return saveIDs.find(entry.id) != saveIDs.end();
                });

            auto size = save.size();
            archive(size);

            for (auto& stored : save)
            {
                if (saveIDs.find(stored.id) == saveIDs.end())
                    continue;

                archive(stored.id);
                archive(stored.shard);
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
                    archive(*foundShard);
                else
                {
                    auto shard = Create<ShardT>();
                    archive(shard);
                    object.list.emplace_back(id, std::move(shard));
                }

                matrixSnapshot.Finalize();
            }
        }
    }

    template<class T>
    template<class U, std::enable_if_t<!Arca::HasScribe<U, BinaryArchive>(), int>>
    void Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>>::
        DoScriven(ObjectT&, BinaryArchive&)
    {}

    template<class T>
    template<class U, std::enable_if_t<Arca::HasScribe<U, JsonArchive>(), int>>
    void Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>>::
        DoScriven(const std::string& name, ObjectT& object, JsonArchive& archive)
    {
        if (archive.IsOutput())
        {
            auto& saveIDs = archive.UserContext<SaveUserContext>()->ids;

            typename ObjectT::List save;
            std::copy_if(
                object.list.begin(),
                object.list.end(),
                std::back_inserter(save),
                [&saveIDs](const typename ObjectT::Entry& entry)
                {
                    return saveIDs.find(entry.id) != saveIDs.end();
                });

            auto output = archive.AsOutput();
            output->StartList(name);
            for (auto& stored : save)
            {
                output->StartObject("");
                archive("id", stored.id);
                archive("shard", stored.shard);
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

                auto matrixSnapshot = object.owner->matrices.StartCreationTransaction(id);

                auto foundShard = object.Find(id);
                if (foundShard)
                    archive("shard", *foundShard);
                else
                {
                    auto shard = Create<ShardT>();
                    archive("shard", shard);
                    object.list.emplace_back(id, std::move(shard));
                }

                matrixSnapshot.Finalize();

                input->EndObject();
            }
            input->EndList();
        }
    }

    template<class T>
    template<class U, std::enable_if_t<!Arca::HasScribe<U, JsonArchive>(), int>>
    void Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>>::
        DoScriven(const std::string& name, ObjectT&, JsonArchive&)
    {}

    template<class T>
    template<class U, std::enable_if_t<Arca::has_shard_serialization_constructor_v<U>, int>>
    U Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>>::Create()
    {
        return ObjectT::StoredT{ Arca::Serialization{} };
    }

    template<class T>
    template<class U, std::enable_if_t<!Arca::has_shard_serialization_constructor_v<U> && Arca::has_shard_default_constructor_v<U>, int>>
    U Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>>::Create()
    {
        return ObjectT::StoredT{};
    }

    template<class T>
    template<class U, std::enable_if_t<!Arca::has_shard_serialization_constructor_v<U> && !Arca::has_shard_default_constructor_v<U>, int>>
    U Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>>::Create()
    {
        static_assert(
            "A shard requires a serialization constructor (taking only the class Serialization) "
            "or a default constructor in order to be serialized.");
        return ObjectT::StoredT{};
    }
}