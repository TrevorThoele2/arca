#pragma once

#include "RelicID.h"
#include "RelicInit.h"

#include "IsShard.h"
#include "IsMatrix.h"

#include "Index.h"
#include "Index.h"
#include "Handle.h"

#include "CompositeScribeCategory.h"

namespace Arca
{
    class Reliquary;

    class OpenRelic final
    {
    public:
        explicit OpenRelic(RelicInit init);
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
        RelicID id = nullRelicID;
        Reliquary* owner = nullptr;
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
        static inline const TypeName typeName = "Arca::OpenRelic";
    };
}

namespace Inscription
{
    template<>
    class Scribe<Arca::OpenRelic> final
    {
    public:
        using ObjectT = Arca::OpenRelic;
    public:
        template<class Archive>
        void Scriven(ObjectT&, Archive&)
        {}
    };

    template<class Archive>
    struct ScribeTraits<Arca::OpenRelic, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<Arca::OpenRelic>;
    };
}