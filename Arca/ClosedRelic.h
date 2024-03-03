#pragma once

#include "RelicID.h"

#include "IsShard.h"
#include "IsMatrix.h"

#include "LocalPtr.h"
#include "MatrixPtr.h"
#include "Handle.h"

#include "CompositeScribe.h"

namespace Arca
{
    class ClosedRelic
    {
    public:
        explicit operator bool() const;

        template<class ShardT, std::enable_if_t<is_shard_v<ShardT>, int> = 0>
        [[nodiscard]] LocalPtr<ShardT> Find() const;
        template<class MatrixT, std::enable_if_t<is_matrix_v<MatrixT>, int> = 0>
        [[nodiscard]] MatrixPtr<MatrixT> Find() const;
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
        ClosedRelic() = default;
    private:
        friend Reliquary;
        friend class ReliquaryRelics;
        friend class ReliquaryOrigin;
        template<class, class>
        friend class BatchSource;
    private:
        INSCRIPTION_ACCESS;
    };

    template<>
    struct Traits<ClosedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ClosedRelic";
    };
}

namespace Inscription
{
    template<>
    class Scribe<::Arca::ClosedRelic, BinaryArchive> final :
        public ArcaCompositeScribe<::Arca::ClosedRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
    };
}