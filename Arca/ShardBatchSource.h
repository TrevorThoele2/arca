#pragma once

#include <vector>

#include "BatchSource.h"
#include "IsShard.h"
#include "HasShardDefaultConstructor.h"
#include "HasShardSerializationConstructor.h"

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

        [[nodiscard]] virtual SizeT Size() const = 0;

        [[nodiscard]] virtual Type Type() const = 0;
    };

    template<class T>
    class BatchSource<T, std::enable_if_t<is_shard_v<T>>> : public ShardBatchSourceBase
    {
    public:
        using ShardT = T;
    private:
        using StoredT = std::decay_t<ShardT>;

        struct Entry
        {
            RelicID id;
            StoredT shard;

            Entry(RelicID id, StoredT&& shard);
        };

        using List = std::vector<Entry>;
    public:
        using iterator = typename List::iterator;
        using const_iterator = typename List::const_iterator;
    public:
        explicit BatchSource(Reliquary& owner);
        BatchSource(const BatchSource& arg) = delete;
        BatchSource(BatchSource&& arg) = default;

        template<class... ConstructorArgs>
        ShardT* Add(RelicID id, ConstructorArgs&& ... constructorArgs);

        iterator Destroy(RelicID destroy);
        iterator Destroy(iterator destroy);
        iterator Destroy(const_iterator destroy);

        bool DestroyFromBase(RelicID id) override;

        void Clear();

        [[nodiscard]] ShardT* Find(RelicID id);
        [[nodiscard]] const ShardT* Find(RelicID id) const;

        [[nodiscard]] bool ContainsFromBase(RelicID id) const override;

        [[nodiscard]] SizeT Size() const override;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;

        [[nodiscard]] Arca::Type Type() const override;
    private:
        List list;
    private:
        Reliquary* owner = nullptr;
        friend class Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>> final
    {
    public:
        using ObjectT = Arca::BatchSource<T, std::enable_if_t<Arca::is_shard_v<T>>>;
    public:
        void Scriven(ObjectT& object, BinaryArchive& archive);
        void Scriven(const std::string& name, ObjectT& object, JsonArchive& archive);
    private:
        template<class U, std::enable_if_t<Arca::HasScribe<U, BinaryArchive>(), int> = 0>
        void DoScriven(ObjectT& object, BinaryArchive& archive);
        template<class U, std::enable_if_t<!Arca::HasScribe<U, BinaryArchive>(), int> = 0>
        void DoScriven(ObjectT& object, BinaryArchive& archive);

        template<class U, std::enable_if_t<Arca::HasScribe<U, JsonArchive>(), int> = 0>
        void DoScriven(const std::string& name, ObjectT& object, JsonArchive& archive);
        template<class U, std::enable_if_t<!Arca::HasScribe<U, JsonArchive>(), int> = 0>
        void DoScriven(const std::string& name, ObjectT& object, JsonArchive& archive);

        template<class U, std::enable_if_t<Arca::has_shard_serialization_constructor_v<U>, int> = 0>
        U Create();
        template<class U, std::enable_if_t<!Arca::has_shard_serialization_constructor_v<U> && Arca::has_shard_default_constructor_v<U>, int> = 0>
        U Create();
        template<class U, std::enable_if_t<!Arca::has_shard_serialization_constructor_v<U> && !Arca::has_shard_default_constructor_v<U>, int> = 0>
        U Create();
    private:
        using ShardT = typename ObjectT::ShardT;
    };
}