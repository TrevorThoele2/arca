#pragma once

#include <unordered_map>

#include "BatchSource.h"
#include "RelicID.h"
#include "IsRelic.h"
#include "RelicInit.h"
#include <Chroma/IsBracesConstructible.h>
#include "HasRelicInitConstructor.h"
#include "HasRelicSerializationConstructor.h"

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

        [[nodiscard]] virtual void* FindStorage(RelicID id) = 0;

        virtual void DestroyFromBase(RelicID id) = 0;

        [[nodiscard]] virtual SizeT Size() const = 0;

        [[nodiscard]] virtual Type Type() const = 0;
        [[nodiscard]] virtual Reliquary& Owner() const = 0;
    };

    template<class T>
    class BatchSource<T, std::enable_if_t<is_relic_v<T>>> : public RelicBatchSourceBase
    {
    public:
        using RelicT = T;
    private:
        using Map = std::unordered_map<RelicID, std::shared_ptr<RelicT>>;
    public:
        using iterator = typename Map::iterator;
        using const_iterator = typename Map::const_iterator;
    public:
        explicit BatchSource(Reliquary& owner, ReliquaryShards& shards);
        BatchSource(const BatchSource& arg) = delete;
        BatchSource(BatchSource&& arg) = default;

        template<class... ConstructorArgs>
        RelicT* Create(RelicID id, ConstructorArgs&& ... constructorArgs);

        iterator Destroy(RelicID destroy);
        iterator Destroy(iterator destroy);
        iterator Destroy(const_iterator destroy);

        void DestroyFromBase(RelicID id) override;

        void Clear();

        [[nodiscard]] void* FindStorage(RelicID id) override;
        [[nodiscard]] std::weak_ptr<RelicT> Find(RelicID id);

        [[nodiscard]] SizeT Size() const override;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;

        [[nodiscard]] Arca::Type Type() const override;
        [[nodiscard]] Reliquary& Owner() const override;
    private:
        Map map;
        Reliquary* owner;
        ReliquaryShards* shards;

        template<class U, class... ConstructorArgs>
        RelicT* CreateImpl(RelicInit init, ConstructorArgs&& ... constructorArgs)
        {
            if constexpr (std::is_constructible_v<U, RelicInit, ConstructorArgs...> && !std::is_constructible_v<U, ConstructorArgs...>)
            {
                auto emplaced = map.emplace(init.id, std::make_shared<RelicT>(init, std::forward<ConstructorArgs>(constructorArgs)...));
                return emplaced.first->second.get();
            }
            else if constexpr (std::is_constructible_v<U, ConstructorArgs...>)
            {
                auto emplaced = map.emplace(init.id, std::make_shared<RelicT>(std::forward<ConstructorArgs>(constructorArgs)...));
                return emplaced.first->second.get();
            }
            else
            {
                static_assert("Cannot create Relic.");
                return nullptr;
            }
        }
    private:
        friend class Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    BatchSource<T, std::enable_if_t<is_relic_v<T>>>::BatchSource(Reliquary& owner, ReliquaryShards& shards) :
        owner(&owner), shards(&shards)
    {}

    template<class T>
    template<class... ConstructorArgs>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Create(RelicID id, ConstructorArgs&& ... constructorArgs) -> RelicT*
    {
        auto found = Find(id);
        return found.lock()
            ? nullptr
            : CreateImpl<T>(RelicInit{ id, *owner, *shards }, std::forward<ConstructorArgs>(constructorArgs)...);
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Destroy(RelicID destroy) -> iterator
    {
        auto found = map.find(destroy);
        return found != map.end() ? map.erase(found) : map.end();
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
    void BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Clear()
    {
        map.clear();
    }

    template<class T>
    void* BatchSource<T, std::enable_if_t<is_relic_v<T>>>::FindStorage(RelicID id)
    {
        auto found = map.find(id);
        return found == map.end() ? nullptr : found->second.get();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Find(RelicID id) -> std::weak_ptr<RelicT>
    {
        auto found = map.find(id);
        return found == map.end() ? nullptr : found->second;
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

    template<class T>
    Reliquary& BatchSource<T, std::enable_if_t<is_relic_v<T>>>::Owner() const
    {
        return *owner;
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>> final
    {
    public:
        using ObjectT = Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>;
    public:
        void Scriven(ObjectT& object, Format::Binary& format);
        void Scriven(const std::string& name, ObjectT& object, Format::Json& format);
    private:
        using RelicT = typename ObjectT::RelicT;
        std::shared_ptr<RelicT> Create(Arca::RelicInit init);
    };
    template<class T>
    void Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>>::
        Scriven(ObjectT& object, Format::Binary& format)
    {
        if (format.IsOutput())
        {
            auto size = object.map.size();
            format(size);

            for (auto& entry : object.map)
            {
                auto id = entry.first;
                format(id);
                format(*entry.second);
            }
        }
        else
        {
            ContainerSize size;
            format(size);

            while (size-- > 0)
            {
                Arca::RelicID id = 0;
                format(id);

                auto foundRelic = object.Find(id).lock();
                if (foundRelic)
                    format(*foundRelic);
                else
                {
                    auto relic = Create(Arca::RelicInit{ id, *object.owner, object.owner->shards });
                    format(*relic);
                    auto& emplaced = object.map.emplace(id, std::move(relic)).first->second;
                    format.types.AttemptReplaceTrackedObject(relic, emplaced);
                }
            }
        }
    }

    template<class T>
    void Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>>::Scriven(
        const std::string& name, ObjectT& object, Format::Json& format)
    {
        if (format.IsOutput())
        {
            const auto output = format.AsOutput();
            output->StartList(name);
            size_t i = 0;
            for (auto& entry : object.map)
            {
                output->StartObject(Chroma::ToString(i));
                auto id = entry.first;
                format("id", id);
                format("relic", *entry.second);
                output->EndObject();
            }
            output->EndList();
            ++i;
        }
        else
        {
            const auto input = format.AsInput();
            auto size = input->StartList(name);
            size_t i = 0;
            while (size-- > 0)
            {
                input->StartObject(Chroma::ToString(i));

                Arca::RelicID id = 0;
                format("id", id);

                auto relic = Create(Arca::RelicInit{ id, *object.owner, object.owner->shards });
                format("relic", *relic);
                auto& emplaced = object.map.emplace(id, std::move(relic)).first->second;
                format.types.AttemptReplaceTrackedObject(relic, emplaced);

                input->EndObject();
                ++i;
            }
            input->EndList();
        }
    }

    template<class T>
    auto Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>>::Create(Arca::RelicInit init) -> std::shared_ptr<RelicT>
    {
        using RelicT = typename ObjectT::RelicT;
        if constexpr (Arca::has_relic_init_serialization_constructor_v<RelicT>)
            return std::make_shared<RelicT>(init, Arca::Serialization{});
        else if constexpr (Arca::has_relic_serialization_constructor_v<RelicT>)
            return std::make_shared<RelicT>(Arca::Serialization{});
        else if constexpr (Arca::has_relic_init_constructor_v<RelicT>)
            return std::make_shared<RelicT>(init);
        else if constexpr (Chroma::is_braces_default_constructible_v<RelicT>)
            return std::make_shared<RelicT>();
        else
        {
            static_assert(
                "A relic requires a constructor of form "
                "(RelicInit, Serialization), "
                "(Serialization), "
                "(RelicInit) or "
                "() "
                "in order to be serialized. Order given is priority order.");
            using RelicT = typename ObjectT::RelicT;
            return std::make_shared<RelicT>(init);
        }
    }
}