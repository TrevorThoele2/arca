#pragma once

#include <vector>

#include "KnownPolymorphicSerializer.h"

#include "RelicMetadata.h"
#include "Handle.h"
#include "Ptr.h"
#include "ReliquaryException.h"
#include "Traits.h"

#include "Batch.h"
#include "BatchSource.h"

namespace Arca
{
    struct RelicMetadata;
    class Reliquary;
    class ReliquaryRelics;
    class ReliquaryRelicStructures;
    class ReliquaryShards;
    class ReliquaryCurators;
    class ReliquarySignals;

    class ReliquaryComponent
    {
    public:
        using KnownPolymorphicSerializerList = std::vector<KnownPolymorphicSerializer>;
    public:
        [[nodiscard]] Arca::NotRegistered NotRegistered(
            const Type& type) const;
        [[nodiscard]] Arca::NotRegistered NotRegistered(
            const Type& type,
            const std::type_index& classType) const;
        [[nodiscard]] Arca::CannotCreate CannotCreate(
            const Type& type) const;
        [[nodiscard]] Arca::CannotCreate CannotCreate(
            const Type& type,
            const std::type_index& classType) const;
        [[nodiscard]] Arca::CannotDestroy CannotDestroy(
            const Type& type) const;
        [[nodiscard]] Arca::CannotDestroy CannotDestroy(
            const Type& type,
            const std::type_index& classType) const;
        [[nodiscard]] Arca::CannotFind CannotFind(
            const Type& type) const;
        [[nodiscard]] Arca::CannotFind CannotFind(
            const Type& type,
            const std::type_index& classType) const;
        [[nodiscard]] CannotModifyShards CannotModify(RelicID id) const;
    protected:
        explicit ReliquaryComponent(Reliquary& owner, std::string objectTypeName);
    protected:
        [[nodiscard]] Reliquary& Owner();
        [[nodiscard]] const Reliquary& Owner() const;
        [[nodiscard]] ReliquaryRelics& Relics();
        [[nodiscard]] const ReliquaryRelics& Relics() const;
        [[nodiscard]] ReliquaryRelicStructures& RelicStructures();
        [[nodiscard]] const ReliquaryRelicStructures& RelicStructures() const;
        [[nodiscard]] ReliquaryShards& Shards();
        [[nodiscard]] const ReliquaryShards& Shards() const;
        [[nodiscard]] ReliquaryCurators& Curators();
        [[nodiscard]] const ReliquaryCurators& Curators() const;
        [[nodiscard]] ReliquarySignals& Signals();
        [[nodiscard]] const ReliquarySignals& Signals() const;
    protected:
        Handle HandleFrom(RelicID id, Type type, HandleObjectType objectType);
        Handle HandleFrom(const RelicMetadata& metadata);
        template<class T>
        Ptr<T> PtrFrom(RelicID id) const;
        template<class T>
        Ptr<T> PtrFrom(const RelicMetadata& metadata) const;
    protected:
        template<class BatchSourceBaseT, class Owner, class Derived>
        class StorageBatchSources
        {
        private:
            template<class RelicT>
            constexpr static bool is_object_v = Derived::template is_object_v<RelicT>;
        public:
            using Ptr = std::unique_ptr<BatchSourceBaseT>;
            using Map = std::unordered_map<TypeName, Ptr>;

            Map map;

            [[nodiscard]] BatchSourceBaseT* Find(const TypeName& typeName) const
            {
                const auto found = map.find(typeName);
                if (found == map.end())
                    return nullptr;

                return found->second.get();
            }
            template<class ObjectT, std::enable_if_t<is_object_v<ObjectT>, int> = 0>
            [[nodiscard]] BatchSource<ObjectT>* Find() const
            {
                auto& map = AsDerived().template MapFor<ObjectT>();
                const auto typeName = TypeFor<ObjectT>().name;
                auto found = map.find(typeName);
                if (found == map.end())
                    return nullptr;

                return static_cast<BatchSource<ObjectT>*>(found->second.get());
            }
            template<class ObjectT, std::enable_if_t<is_object_v<ObjectT>, int> = 0>
            BatchSource<ObjectT>& Required() const
            {
                auto found = Find<ObjectT>();
                if (!found)
                {
                    const auto type = TypeFor<ObjectT>();
                    throw owner->NotRegistered(type, typeid(ObjectT));
                }

                return *found;
            }

            template<class ObjectT, std::enable_if_t<is_object_v<ObjectT>, int> = 0>
            Batch<ObjectT> Batch() const
            {
                auto& batchSource = Required<ObjectT>();
                return Arca::Batch<ObjectT>(batchSource);
            }
        protected:
            explicit StorageBatchSources(Owner& owner) : owner(&owner)
            {}
        private:
            Owner* owner;
        private:
            [[nodiscard]] Derived& AsDerived()
            {
                return static_cast<Derived&>(*this);
            }

            [[nodiscard]] const Derived& AsDerived() const
            {
                return static_cast<const Derived&>(*this);
            }
        };

        template<class Key, class BatchSourceBaseT, class Owner, class Derived>
        class MetaBatchSources
        {
        private:
            template<class RelicT>
            constexpr static bool should_accept = Derived::template should_accept<RelicT>;
        public:
            using Ptr = std::unique_ptr<BatchSourceBaseT>;
            using Map = std::unordered_map<Key, Ptr>;

            Map map;

            [[nodiscard]] BatchSourceBaseT* Find(const Key& key) const
            {
                const auto found = map.find(key);
                if (found == map.end())
                    return nullptr;

                return found->second.get();
            }
            template<class ObjectT, std::enable_if_t<should_accept<ObjectT>, int> = 0>
            [[nodiscard]] BatchSource<ObjectT>* Find() const
            {
                auto& map = AsDerived().template MapFor<ObjectT>();
                const auto key = KeyFor<ObjectT>();
                auto found = map.find(key);
                if (found == map.end())
                    return nullptr;

                return static_cast<BatchSource<ObjectT>*>(found->second.get());
            }
            template<class ObjectT, std::enable_if_t<should_accept<ObjectT>, int> = 0>
            BatchSource<ObjectT>& Required()
            {
                auto found = Find<ObjectT>();
                if (found)
                    return *found;

                const auto key = KeyFor<ObjectT>();
                auto batchSource = AsDerived().template Create<ObjectT>();
                auto emplaced = map.emplace(key, std::move(batchSource)).first->second.get();
                return static_cast<BatchSource<ObjectT>&>(*emplaced);
            }

            template<class ObjectT, std::enable_if_t<should_accept<ObjectT>, int> = 0>
            Batch<ObjectT> Batch()
            {
                auto& batchSource = Required<ObjectT>();
                return Arca::Batch<ObjectT>(batchSource);
            }
        protected:
            explicit MetaBatchSources(Owner& owner) : owner(&owner)
            {}
        protected:
            Owner* owner;
        private:
            [[nodiscard]] Derived& AsDerived()
            {
                return static_cast<Derived&>(*this);
            }

            [[nodiscard]] const Derived& AsDerived() const
            {
                return static_cast<const Derived&>(*this);
            }

            template<class T>
            [[nodiscard]] static Key KeyFor()
            {
                return Derived::template KeyFor<T>();
            }
        };
    private:
        Reliquary* owner;
    private:
        const std::string objectTypeName;

        template<class ExceptionT, class... Args>
        ExceptionT CreateException(Args&& ... args) const;
    private:
        friend Reliquary;
    };

    template<class T>
    Ptr<T> ReliquaryComponent::PtrFrom(RelicID id) const
    {
        return Ptr<T>(id, const_cast<Reliquary&>(Owner()));
    }

    template<class T>
    Ptr<T> ReliquaryComponent::PtrFrom(const RelicMetadata& metadata) const
    {
        return Ptr<T>(metadata.id, const_cast<Reliquary&>(Owner()));
    }

    template<class ExceptionT, class... Args>
    ExceptionT ReliquaryComponent::CreateException(Args&& ... args) const
    {
        return ExceptionT(objectTypeName, std::forward<Args>(args)...);
    }
}
