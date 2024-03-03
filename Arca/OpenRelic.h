#pragma once

#include "RelicID.h"
#include "RelicInit.h"

#include "IsShard.h"
#include "IsMatrix.h"

#include "ShardIndex.h"
#include "MatrixIndex.h"
#include "Handle.h"

#include "CompositeScribe.h"

namespace Arca
{
    class Reliquary;

    class OpenRelic final
    {
    public:
        template<class ShardT, class... ConstructorArgs, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        Index<ShardT> Create(ConstructorArgs&& ... constructorArgs) const;
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        void Destroy() const;
        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        void Destroy() const;
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] Index<ShardT> Find() const;
        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        [[nodiscard]] Index<MatrixT> Find() const;
        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] bool Contains() const;
        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        [[nodiscard]] bool Contains() const;

        [[nodiscard]] std::optional<Handle> Parent() const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Reliquary& Owner() const;
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
    private:
        explicit OpenRelic(RelicInit init);
    private:
        friend class ReliquaryRelics;
        template<class, class>
        friend class BatchSource;
    private:
        INSCRIPTION_ACCESS;
    };

    template<>
    struct Traits<OpenRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "OpenRelic";
    };
}

namespace Inscription
{
    template<>
    class Scribe<::Arca::OpenRelic, BinaryArchive> final :
        public ArcaCompositeScribe<::Arca::OpenRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };
}