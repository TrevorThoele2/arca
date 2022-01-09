#pragma once

#include "ShardBatchSource.h"
#include "Reliquary.h"
#include "ReliquaryShards.h"

namespace Inscription
{
    template<class T>
    void Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>>::Scriven(
        ObjectT& object, Format::Binary& format)
    {
        if (format.IsOutput())
        {
            auto& saveIDs = format.UserContext<SaveUserContext>()->ids;

            std::vector<typename ObjectT::iterator> save;
            for (auto& id : saveIDs)
            {
                auto found = object.map.find(id);
                if (found != object.map.end())
                    save.push_back(found);
            }

            auto size = save.size();
            format(size);

            for (auto& stored : save)
            {
                if (saveIDs.find(stored->first) != saveIDs.end())
                {
                    format(stored->first);
                    format(*stored->second.shard);
                    format(stored->second.required);
                }
            }
        }
        else
        {
            ContainerSize size;
            format(size);

            while (size-- > 0)
            {
                Arca::RelicID id;
                format(id);

                auto foundShard = object.Find(id).lock();
                if (foundShard)
                {
                    format(*foundShard);
                    bool required = false;
                    format(required);
                }
                else
                {
                    auto shard = Create();
                    format(*shard);
                    bool required = false;
                    format(required);
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
        const std::string& name, ObjectT& object, Format::Json& format)
    {
        if (format.IsOutput())
        {
            auto& saveIDs = format.UserContext<SaveUserContext>()->ids;

            std::vector<typename ObjectT::iterator> save;
            for (auto& id : saveIDs)
            {
                auto found = object.map.find(id);
                if (found != object.map.end())
                    save.push_back(found);
            }

            const auto output = format.AsOutput();
            output->StartList(name);
            size_t i = 0;
            for (auto& stored : save)
            {
                output->StartObject(Chroma::ToString(i));
                format("id", stored->first);
                format("shard", *stored->second.shard);
                format("required", stored->second.required);
                output->EndObject();
                ++i;
            }
            output->EndList();
        }
        else
        {
            const auto input = format.AsInput();
            auto size = input->StartList(name);
            size_t i = 0;
            while (size-- > 0)
            {
                input->StartObject(Chroma::ToString(i));

                Arca::RelicID id = Arca::nullRelicID;
                format("id", id);

                auto foundShard = object.Find(id).lock();
                if (foundShard)
                {
                    format("shard", *foundShard);
                    bool required = false;
                    format("required", required);
                }
                else
                {
                    auto shard = Create();
                    format("shard", *shard);
                    bool required = false;
                    format("required", required);
                    object.map.emplace(id, typename ObjectT::StoredT{ required, shard });
                }

                const auto type = Arca::TypeFor<T>();
                object.shards->AddType(id, type);
                object.matrices->ShardCreated(id, type, object.shards->AllTypes(id));

                input->EndObject();

                ++i;
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
                false,
                "A shard requires a serialization constructor (taking only the class Serialization) "
                "or a default constructor in order to be serialized.");
            return std::make_shared<CreateT>();
        }
    }
}