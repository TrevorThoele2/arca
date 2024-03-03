#pragma once

#include <unordered_map>

#include "BatchSource.h"
#include "RelicID.h"
#include "IsRelic.h"
#include "RelicInit.h"
#include "HasRelicDefaultConstructor.h"
#include "HasRelicSerializationConstructor.h"

#include "Serialization.h"
#include "HasScribe.h"

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
        virtual void DestroyAllFromBase(Reliquary& reliquary) = 0;

        virtual void SetOwner(Reliquary& owner) = 0;

        [[nodiscard]] virtual SizeT Size() const = 0;

        [[nodiscard]] virtual Type Type() const = 0;
    };

    template<class T>
    class BatchSource<T, std::enable_if_t<is_relic_v<T>>> : public RelicBatchSourceBase
    {
    public:
        using RelicT = T;
    private:
        using Map = std::unordered_map<RelicID, RelicT>;
    public:
        using iterator = typename Map::iterator;
        using const_iterator = typename Map::const_iterator;
    public:
        explicit BatchSource(Reliquary& owner);
        BatchSource(const BatchSource& arg) = delete;
        BatchSource(BatchSource&& arg) = default;

        template<class... ConstructorArgs>
        RelicT* Add(RelicInit init, ConstructorArgs&& ... constructorArgs);

        iterator Destroy(RelicID destroy);
        iterator Destroy(iterator destroy);
        iterator Destroy(const_iterator destroy);

        void DestroyFromBase(RelicID id) override;
        void DestroyAllFromBase(Reliquary& reliquary) override;

        void Clear();

        void SetOwner(Reliquary& owner) override;

        [[nodiscard]] void* FindStorage(RelicID id) override;
        [[nodiscard]] RelicT* Find(RelicID id);

        [[nodiscard]] SizeT Size() const override;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;

        [[nodiscard]] Arca::Type Type() const override;
    private:
        Map map;
        Reliquary* owner;
    private:
        friend class Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>> final
    {
    public:
        using ObjectT = Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>;
    public:
        void Scriven(ObjectT& object, BinaryArchive& archive);
        void Scriven(const std::string& name, ObjectT& object, JsonArchive& archive);
    private:
        template<class U, std::enable_if_t<Arca::has_relic_serialization_constructor_v<U>, int> = 0>
        U Create(Arca::RelicInit init);
        template<class U, std::enable_if_t<!Arca::has_relic_serialization_constructor_v<U> && Arca::has_relic_default_constructor_v<U>, int> = 0>
        U Create(Arca::RelicInit init);
        template<class U, std::enable_if_t<!Arca::has_relic_serialization_constructor_v<U> && !Arca::has_relic_default_constructor_v<U>, int> = 0>
        U Create(Arca::RelicInit init);
    private:
        using RelicT = typename ObjectT::RelicT;
    };
}