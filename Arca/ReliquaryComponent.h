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
    class ReliquaryShards;
    class ReliquaryCurators;
    class ReliquarySignals;

    class ReliquaryComponent
    {
    public:
        using KnownPolymorphicSerializerList = std::vector<KnownPolymorphicSerializer>;
    public:
        [[nodiscard]] Arca::NotRegistered NotRegistered(
            const TypeHandle& type) const;
        [[nodiscard]] Arca::NotRegistered NotRegistered(
            const TypeHandle& type,
            const std::type_index& classType) const;
        [[nodiscard]] CannotModify CannotModify(RelicID id) const;
    protected:
        explicit ReliquaryComponent(Reliquary& owner, std::string objectTypeName);
    protected:
        [[nodiscard]] Reliquary& Owner();
        [[nodiscard]] const Reliquary& Owner() const;
        [[nodiscard]] ReliquaryRelics& Relics();
        [[nodiscard]] const ReliquaryRelics& Relics() const;
        [[nodiscard]] ReliquaryShards& Shards();
        [[nodiscard]] const ReliquaryShards& Shards() const;
        [[nodiscard]] ReliquaryCurators& Curators();
        [[nodiscard]] const ReliquaryCurators& Curators() const;
        [[nodiscard]] ReliquarySignals& Signals();
        [[nodiscard]] const ReliquarySignals& Signals() const;
    protected:
        Handle HandleFrom(RelicID id);
        Handle HandleFrom(const RelicMetadata& metadata);
        template<class T>
        Ptr<T> PtrFrom(RelicID id);
        template<class T>
        Ptr<T> PtrFrom(const RelicMetadata& metadata);
    protected:
        template<class BatchSourceBaseT, class Owner, class Derived>
        class BatchSourcesBase
        {
        private:
            template<class RelicT>
            constexpr static bool is_object_v = Derived::template is_object_v<RelicT>;
        public:
            using Ptr = std::unique_ptr<BatchSourceBaseT>;
            using Map = std::unordered_map<TypeHandleName, Ptr>;

            Map map;

            [[nodiscard]] BatchSourceBaseT* Find(const TypeHandleName& typeHandle) const
            {
                const auto found = map.find(typeHandle);
                if (found == map.end())
                    return nullptr;

                return found->second.get();
            }
            template<class ObjectT, std::enable_if_t<is_object_v<ObjectT>, int> = 0>
            [[nodiscard]] BatchSource<ObjectT>* Find() const
            {
                auto& map = AsDerived().template MapFor<ObjectT>();
                const auto typeHandleName = TypeHandleFor<ObjectT>().name;
                auto found = map.find(typeHandleName);
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
                    const auto typeHandle = TypeHandleFor<ObjectT>();
                    throw owner->NotRegistered(typeHandle, typeid(ObjectT));
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
            explicit BatchSourcesBase(Owner& owner) : owner(&owner)
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
    private:
        Reliquary* owner;
    private:
        const std::string objectTypeName;
    private:
        friend Reliquary;
    };

    template<class T>
    Ptr<T> ReliquaryComponent::PtrFrom(RelicID id)
    {
        return Ptr<T>(id, Owner());
    }

    template<class T>
    Ptr<T> ReliquaryComponent::PtrFrom(const RelicMetadata& metadata)
    {
        return Ptr<T>(metadata.id, Owner());
    }
}
