#pragma once

#include "ShardBatchSource.h"
#include "Reliquary.h"
#include "ReliquaryShards.h"

namespace Inscription
{
    template<class T>
    void Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>>::Scriven(
        ObjectT& object, Archive::Binary& archive)
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
                    archive(*stored->second.shard);
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

                auto foundShard = object.Find(id).lock();
                if (foundShard)
                {
                    archive(*foundShard);
                    bool required = false;
                    archive(required);
                }
                else
                {
                    auto shard = Create();
                    archive(*shard);
                    bool required = false;
                    archive(required);
                    object.map.emplace(id, typename ObjectT::StoredT{ required, shard });
                }

                const auto type = Arca::TypeFor<T>();
                object.shards->AddType(id, type);
                object.matrices->ShardCreated(id, type, object.shards->AllTypes(id));
            }
        }
    }

    template<class T>
    void Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>>::Scriven(
        const std::string& name, ObjectT& object, Archive::Json& archive)
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

            auto output = archive.AsOutput();
            output->StartList(name);
            for (auto& stored : save)
            {
                output->StartObject("");
                archive("id", stored->first);
                archive("shard", *stored->second.shard);
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

                auto foundShard = object.Find(id).lock();
                if (foundShard)
                {
                    archive("shard", *foundShard);
                    bool required = false;
                    archive("required", required);
                }
                else
                {
                    auto shard = Create();
                    archive("shard", *shard);
                    bool required = false;
                    archive("required", required);
                    object.map.emplace(id, typename ObjectT::StoredT{ required, shard });
                }

                const auto type = Arca::TypeFor<T>();
                object.shards->AddType(id, type);
                object.matrices->ShardCreated(id, type, object.shards->AllTypes(id));

                input->EndObject();
            }
            input->EndList();
        }
    }

    template<class T>
    auto Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>>::Create() -> std::shared_ptr<CreateT>
    {
        using CreateT = std::decay_t<typename ObjectT::ShardT>;
        if constexpr (Arca::has_shard_serialization_constructor_v<ShardT>)
            return std::make_shared<CreateT>(Arca::Serialization{});
        else if constexpr (Arca::has_shard_default_constructor_v<ShardT>)
            return std::make_shared<CreateT>();
        else
        {
            static_assert(
                "A shard requires a serialization constructor (taking only the class Serialization) "
                "or a default constructor in order to be serialized.");
            return std::make_shared<CreateT>();
        }
    }
}