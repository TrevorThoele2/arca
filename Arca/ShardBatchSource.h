#pragma once

#include <unordered_map>

#include "BatchSource.h"
#include "IsShard.h"
#include "HasShardDefaultConstructor.h"
#include "HasShardSerializationConstructor.h"

#include "ReliquaryMatrices.h"
#include "RelicID.h"
#include "TypeFor.h"

#include "Serialization.h"
#include "HasScribe.h"
#include "SaveUserContext.h"

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
        [[nodiscard]] virtual bool IsRequired(RelicID id) const = 0;

        [[nodiscard]] virtual SizeT Size() const = 0;

        [[nodiscard]] virtual Type Type() const = 0;
    };

    template<class T>
    class BatchSource<T, std::enable_if_t<is_shard_v<T>>> : public ShardBatchSourceBase
    {
    public:
        using ShardT = T;
    private:
        struct StoredT
        {
            bool required;
            std::shared_ptr<std::decay_t<ShardT>> shard;
        };

        using Map = std::unordered_map<RelicID, StoredT>;
    public:
        using iterator = typename Map::iterator;
        using const_iterator = typename Map::const_iterator;
    public:
        explicit BatchSource(Reliquary& owner, ReliquaryMatrices& matrices);
        BatchSource(const BatchSource& arg) = delete;
        BatchSource(BatchSource&& arg) noexcept = default;

        BatchSource& operator=(const BatchSource& arg) = delete;
        BatchSource& operator=(BatchSource&& arg) noexcept = default;

        template<class... ConstructorArgs>
        ShardT* Add(RelicID id, bool required, ConstructorArgs&& ... constructorArgs);

        iterator Destroy(RelicID destroy);
        iterator Destroy(iterator destroy);
        iterator Destroy(const_iterator destroy);

        bool DestroyFromBase(RelicID id) override;

        void Clear();

        [[nodiscard]] std::weak_ptr<ShardT> Find(RelicID id) const;

        [[nodiscard]] bool ContainsFromBase(RelicID id) const override;
        [[nodiscard]] bool IsRequired(RelicID id) const override;

        [[nodiscard]] SizeT Size() const override;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;

        [[nodiscard]] Arca::Type Type() const override;
    private:
        Map map;
    private:
        Reliquary* owner = nullptr;
        ReliquaryMatrices* matrices = nullptr;
        friend class Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    BatchSource<T, std::enable_if_t<is_shard_v<T>>>::BatchSource(Reliquary& owner, ReliquaryMatrices& matrices) :
        owner(&owner), matrices(&matrices)
    {}

    template<class T>
    template<class... ConstructorArgs>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Add(RelicID id, bool required, ConstructorArgs&& ... constructorArgs) -> ShardT*
    {
        auto found = Find(id).lock();
        return found
            ? found.get()
            : map.emplace(
                id,
                StoredT{ required, std::make_shared<std::decay_t<ShardT>>(std::forward<ConstructorArgs>(constructorArgs)...) })
                    .first->second.shard.get();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Destroy(RelicID destroy) -> iterator
    {
        auto found = map.find(destroy);
        return found != map.end() ? map.erase(found) : map.end();
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
    auto BatchSource<T, std::enable_if_t<is_shard_v<T>>>::Find(RelicID id) const -> std::weak_ptr<ShardT>
    {
        auto found = map.find(id);
        return found != map.end() ? found->second.shard : nullptr;
    }
    
    template<class T>
    bool BatchSource<T, std::enable_if_t<is_shard_v<T>>>::ContainsFromBase(RelicID id) const
    {
        return static_cast<bool>(Find(id).lock());
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
    class Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>> final
    {
    public:
        using ObjectT = Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>;
    public:
        void Scriven(ObjectT& object, Archive::Binary& archive);
        void Scriven(const std::string& name, ObjectT& object, Archive::Json& archive);
    private:
        using ShardT = typename ObjectT::ShardT;
        using CreateT = std::decay_t<ShardT>;
        std::shared_ptr<CreateT> Create();
    };

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

                auto matrixSnapshot = object.matrices->StartCreationTransaction(id);

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

                matrixSnapshot.Finalize();
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

                auto matrixSnapshot = object.matrices->StartCreationTransaction(id);

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

                matrixSnapshot.Finalize();

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