#pragma once

#include "ReliquaryComponent.h"

#include "ShardBatch.h"

#include "IsShard.h"

#include "Index.h"

#include "RelicID.h"
#include "Type.h"
#include <Chroma/VariadicTemplate.h>

namespace Arca
{
    class Reliquary;

    class ReliquaryShards : public ReliquaryComponent
    {
    public:
        void Create(const Type& type, RelicID id);
        template<class ShardT, class... ConstructorArgs>
        Index<ShardT> Create(RelicID id, ConstructorArgs&& ... constructorArgs);
        template<class ShardT, class... ConstructorArgs>
        Index<ShardT> CreateFromInternal(RelicID id, ConstructorArgs&& ... constructorArgs);

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        void Destroy(RelicID id);
        void TransactionalDestroy(const Type& type, RelicID id);
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        void TransactionalDestroy(RelicID id);
        void Clear();
        void Clear(RelicID id);

        [[nodiscard]] bool Contains(const Handle& handle) const;
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] bool Contains(RelicID id) const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] RelicID IDFor(const ShardT& shard) const;

        [[nodiscard]] bool IsShardTypeName(const TypeName& typeName) const;
        [[nodiscard]] std::vector<TypeName> AllTypeNames() const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] ShardT* FindStorage(RelicID id);

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        void SignalCreation(const Index<ShardT>& index);
    public:
        class HandlerBase : public KnownPolymorphicSerializer
        {
        public:
            const TypeName typeName;
        public:
            virtual ~HandlerBase() = 0;

            virtual ShardBatchSourceBase& BatchSource() = 0;
            virtual ShardBatchSourceBase& ConstBatchSource() = 0;

            virtual void Create(RelicID id, Reliquary& reliquary, bool isConst) = 0;
            virtual void RequiredDestroy(RelicID id, Reliquary& reliquary) = 0;
            virtual void Clear() = 0;

            [[nodiscard]] virtual bool Contains(RelicID id) const = 0;

            [[nodiscard]] TypeName MainType() const override;
        protected:
            explicit HandlerBase(const TypeName& typeName);
        };

        template<class ShardT>
        class Handler final : public HandlerBase
        {
        public:
            Arca::BatchSource<ShardT> batchSource;
            Arca::BatchSource<const ShardT> constBatchSource;
        public:
            explicit Handler(Reliquary& reliquary);

            ShardBatchSourceBase& BatchSource() override;
            ShardBatchSourceBase& ConstBatchSource() override;

            void Create(RelicID id, Reliquary& reliquary, bool isConst) override;
            template<class... ConstructorArgs>
            void CreateCommon(RelicID id, Reliquary& reliquary, bool isConst, ConstructorArgs&& ... constructorArgs);
            void RequiredDestroy(RelicID id, Reliquary& reliquary) override;
            void Clear() override;

            [[nodiscard]] bool Contains(RelicID id) const override;

            [[nodiscard]] bool WillBinarySerialize() const override;
            [[nodiscard]] bool WillJsonSerialize() const override;
            void Serialize(Inscription::BinaryArchive& archive) override;
            void Serialize(const std::string& name, Inscription::JsonArchive& archive) override;
            [[nodiscard]] std::vector<::Inscription::Type> InscriptionTypes(Inscription::BinaryArchive& archive) const override;
            [[nodiscard]] std::vector<::Inscription::Type> InscriptionTypes(Inscription::JsonArchive& archive) const override;
        };

        using HandlerPtr = std::unique_ptr<HandlerBase>;
        using HandlerList = std::vector<HandlerPtr>;
        HandlerList handlers;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        void CreateHandler();
        [[nodiscard]] HandlerBase* FindHandler(const TypeName& typeName) const;
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] Handler<ShardT>* FindHandler() const;

        [[nodiscard]] ShardBatchSourceBase* FindBatchSource(const Type& type) const;
        template<class ObjectT, std::enable_if_t<is_shard_v<ObjectT>, int> = 0>
        [[nodiscard]] BatchSource<ObjectT>* FindBatchSource() const;

        [[nodiscard]] ShardBatchSourceBase& RequiredBatchSource(const Type& type) const;
        template<class ObjectT, std::enable_if_t<is_shard_v<ObjectT>, int> = 0>
        [[nodiscard]] BatchSource<ObjectT>& RequiredBatchSource() const;

        template<class ObjectT, std::enable_if_t<is_shard_v<ObjectT>, int> = 0>
        [[nodiscard]] Arca::Batch<ObjectT> Batch() const;
    public:
        ReliquaryShards(const ReliquaryShards& arg) = delete;
        ReliquaryShards& operator=(const ReliquaryShards& arg) = delete;
    private:
        template<class ShardT, class... ConstructorArgs>
        Index<ShardT> CreateCommon(RelicID id, ConstructorArgs&& ... constructorArgs);
    private:
        template<class T>
        [[nodiscard]] auto CreateIndex(RelicID id) const;
    private:
        template<::Chroma::VariadicTemplateSize i>
        struct DestroyAllShardsIterator
        {
            template<class ShardPack>
            static void Do(ShardPack, RelicID id, ReliquaryShards& shards)
            {
                using T = typename ShardPack::template Parameter<i>::Type;

                shards.TransactionalDestroy<T>(id);
            }
        };

        template<::Chroma::VariadicTemplateSize i>
        struct ContainsAllShardsIterator
        {
            template<class ShardPack>
            static bool Check(ShardPack, RelicID id, const ReliquaryShards& shards)
            {
                using T = typename ShardPack::template Parameter<i>::Type;

                return shards.Contains<T>(id);
            }
        };
    private:
        explicit ReliquaryShards(Reliquary& owner);
        ReliquaryShards(ReliquaryShards&& arg) noexcept = default;
        friend Reliquary;
    };
}