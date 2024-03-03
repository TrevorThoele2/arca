#pragma once

#include <vector>

#include "KnownPolymorphicSerializer.h"

#include "RelicMetadata.h"
#include "Handle.h"
#include "ReliquaryException.h"
#include "Traits.h"
#include "TypeFor.h"

#include "Batch.h"
#include "BatchSource.h"

namespace Arca
{
    struct RelicMetadata;
    class Reliquary;
    class ReliquaryRelics;
    class ReliquaryRelicStructures;
    class ReliquaryShards;
    class ReliquaryMatrices;
    class ReliquaryCurators;
    class ReliquarySignals;
    class ReliquaryCommands;

    class ReliquaryComponent
    {
    public:
        using KnownPolymorphicSerializerList = std::vector<KnownPolymorphicSerializer*>;
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
    public:
        [[nodiscard]] Reliquary& Owner();
        [[nodiscard]] const Reliquary& Owner() const;
    protected:
        explicit ReliquaryComponent(Reliquary& owner, std::string objectTypeName);
    protected:
        [[nodiscard]] ReliquaryRelics& Relics();
        [[nodiscard]] const ReliquaryRelics& Relics() const;
        [[nodiscard]] ReliquaryRelicStructures& RelicStructures();
        [[nodiscard]] const ReliquaryRelicStructures& RelicStructures() const;
        [[nodiscard]] ReliquaryShards& Shards();
        [[nodiscard]] const ReliquaryShards& Shards() const;
        [[nodiscard]] ReliquaryMatrices& Matrices();
        [[nodiscard]] const ReliquaryMatrices& Matrices() const;
        [[nodiscard]] ReliquaryCurators& Curators();
        [[nodiscard]] const ReliquaryCurators& Curators() const;
        [[nodiscard]] ReliquarySignals& Signals();
        [[nodiscard]] const ReliquarySignals& Signals() const;
        [[nodiscard]] ReliquaryCommands& Commands();
        [[nodiscard]] const ReliquaryCommands& Commands() const;
    protected:
        [[nodiscard]] Handle HandleFrom(RelicID id, Type type, HandleObjectType objectType) const;
        [[nodiscard]] Handle HandleFrom(const RelicMetadata& metadata) const;
    protected:
        template<class BatchSourceBaseT, class Owner, class Derived, template<class> class is_object>
        class StorageBatchSourcesBase
        {
        public:
            using Entry = std::unique_ptr<BatchSourceBaseT>;
            using Map = std::unordered_map<TypeName, Entry>;

            Map map;

            [[nodiscard]] BatchSourceBaseT* Find(const TypeName& typeName) const
            {
                const auto found = map.find(typeName);
                if (found == map.end())
                    return nullptr;

                return found->second.get();
            }

            template<class ObjectT, std::enable_if_t<is_object<ObjectT>::value, int> = 0>
            [[nodiscard]] BatchSource<ObjectT>* Find() const
            {
                auto& map = AsDerived().template MapFor<ObjectT>();
                const auto typeName = TypeFor<ObjectT>().name;
                auto found = map.find(typeName);
                if (found == map.end())
                    return nullptr;

                return static_cast<BatchSource<ObjectT>*>(found->second.get());
            }

            [[nodiscard]] BatchSourceBaseT& Required(const TypeName& typeName) const
            {
                auto found = Find(typeName);
                if (!found)
                    throw owner->NotRegistered(Type(typeName));

                return *found;
            }

            template<class ObjectT, std::enable_if_t<is_object<ObjectT>::value, int> = 0>
            [[nodiscard]] BatchSource<ObjectT>& Required() const
            {
                auto found = Find<ObjectT>();
                if (!found)
                {
                    const auto type = TypeFor<ObjectT>();
                    throw owner->NotRegistered(type, typeid(ObjectT));
                }

                return *found;
            }

            template<class ObjectT, std::enable_if_t<is_object<ObjectT>::value, int> = 0>
            [[nodiscard]] Arca::Batch<ObjectT> Batch() const
            {
                auto& batchSource = Required<ObjectT>();
                return Arca::Batch<ObjectT>(batchSource);
            }
        protected:
            explicit StorageBatchSourcesBase(Owner& owner) : owner(&owner)
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

        template<class ExceptionT, class... Args>
        ExceptionT CreateException(Args&& ... args) const;
    private:
        friend Reliquary;
    };

    template<class ExceptionT, class... Args>
    ExceptionT ReliquaryComponent::CreateException(Args&& ... args) const
    {
        return ExceptionT(objectTypeName, std::forward<Args>(args)...);
    }
}
