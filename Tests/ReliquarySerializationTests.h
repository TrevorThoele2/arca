#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelic.h>
#include <Arca/OpenTypedRelic.h>

#include <Inscription/BinaryArchive.h>

using namespace Arca;

class ReliquarySerializationTestsFixture : public GeneralFixture
{
public:
    class BasicShard
    {
    public:
        std::string myValue;
    public:
        BasicShard() = default;
        explicit BasicShard(std::string myValue);
    };

    class BasicShardWithDifferentInputHandle
    {
    public:
        std::string myValue;
    public:
        BasicShardWithDifferentInputHandle() = default;
        explicit BasicShardWithDifferentInputHandle(int myValue);
    };

    class OtherShard
    {
    public:
        int myValue;
    public:
        OtherShard() = default;
        explicit OtherShard(int myValue);
    };

    class TypedClosedRelic final : public ClosedTypedRelic<TypedClosedRelic>
    {
    public:
        int myInt = 0;
        ShardIndex<BasicShard> basicShard;
    public:
        explicit TypedClosedRelic(Initialization initialization);
        TypedClosedRelic(Initialization initialization, int myInt);
    };

    class TypedOpenRelic final : public OpenTypedRelic<TypedOpenRelic>
    {
    public:
        int myInt = 0;
        ShardIndex<BasicShard> basicShard;
    public:
        explicit TypedOpenRelic(Initialization initialization);
        TypedOpenRelic(Initialization initialization, int myInt);
    };

    class GlobalRelic final : public ClosedTypedRelic<GlobalRelic>
    {
    public:
        int myInt = 0;
        ShardIndex<BasicShard> basicShard;
    public:
        explicit GlobalRelic(Initialization initialization);
        GlobalRelic(Initialization initialization, int myInt, std::string shardData);
    };

    class BasicCurator final : public Curator
    {
    public:
        int myInt = 0;

        using Curator::Curator;
    };

    struct BasicSignal
    {};

    class BasicShardNullInscription
    {
    public:
        std::string myValue;
    public:
        BasicShardNullInscription() = default;
        explicit BasicShardNullInscription(std::string myValue);
    };

    class OtherShardNullInscription
    {
    public:
        int myValue;
    public:
        explicit OtherShardNullInscription(int myValue = 0);
    };

    class TypedClosedRelicNullInscription final :
        public ClosedTypedRelic<TypedClosedRelicNullInscription>
    {
    public:
        int myInt = 0;
        ShardIndex<BasicShardNullInscription> basicShard;
    public:
        explicit TypedClosedRelicNullInscription(Initialization initialization);
        TypedClosedRelicNullInscription(Initialization initialization, int myInt);
    };

    class TypedOpenRelicNullInscription final :
        public OpenTypedRelic<TypedOpenRelicNullInscription>
    {
    public:
        int myInt = 0;
        ShardIndex<BasicShardNullInscription> basicShard;
    public:
        explicit TypedOpenRelicNullInscription(Initialization initialization);
        TypedOpenRelicNullInscription(Initialization initialization, int myInt);
    };

    class GlobalRelicNullInscription final :
        public ClosedTypedRelic<GlobalRelicNullInscription>
    {
    public:
        int myInt = 0;
        ShardIndex<BasicShardNullInscription> basicShard;
    public:
        explicit GlobalRelicNullInscription(Initialization initialization);
        GlobalRelicNullInscription(Initialization initialization, int myInt, std::string shardData);
    };

    class MovableOnlyRelic final : public ClosedTypedRelic<MovableOnlyRelic>
    {
    public:
        ShardIndex<BasicShard> basicShard;

        int myValue = 0;
    public:
        explicit MovableOnlyRelic(Initialization initialization);
        MovableOnlyRelic(Initialization initialization, int myInt, std::string shardData);
        MovableOnlyRelic(const MovableOnlyRelic& arg) = delete;
        MovableOnlyRelic(MovableOnlyRelic&& arg) noexcept = default;
        MovableOnlyRelic& operator=(const MovableOnlyRelic& arg) = delete;
        MovableOnlyRelic& operator=(MovableOnlyRelic&& arg) noexcept = default;
    };

    class BasicCuratorNullInscription final : public Curator
    {
    public:
        int myInt = 0;

        using Curator::Curator;
    };

    struct BasicSignalNullInscription
    {};
};

namespace Arca
{
    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ReliquarySerializationTestsBasicShard";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicShardWithDifferentInputHandle>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ReliquarySerializationTestsBasicShardWithDifferentInputHandle";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ReliquarySerializationTestsOtherShard";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::TypedClosedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquarySerializationTestsTypedClosedRelic";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::TypedOpenRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquarySerializationTestsTypedOpenRelic";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquarySerializationTestsGlobalRelic";
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicCurator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "ReliquarySerializationTestsBasicCurator";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicSignal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "ReliquarySerializationTestsBasicSignal";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicShardNullInscription>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ReliquarySerializationTestsBasicShardNullInscription";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::OtherShardNullInscription>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ReliquarySerializationTestsOtherShardNullInscription";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::TypedClosedRelicNullInscription>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquarySerializationTestsTypedClosedRelicNullInscription";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::TypedOpenRelicNullInscription>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquarySerializationTestsTypedOpenRelicNullInscription";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::GlobalRelicNullInscription>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquarySerializationTestsGlobalRelicNullInscription";
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::MovableOnlyRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquarySerializationTestsMovableOnlyRelic";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicCuratorNullInscription>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "ReliquarySerializationTestsBasicCuratorNullInscription";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicSignalNullInscription>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "ReliquarySerializationTestsBasicSignalNullInscription";
    };
}

namespace Inscription
{
    template<>
    class Scribe<::ReliquarySerializationTestsFixture::BasicShard, BinaryArchive> final
        : public ArcaCompositeScribe<::ReliquarySerializationTestsFixture::BasicShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::BasicShardWithDifferentInputHandle, BinaryArchive> final
        : public ArcaCompositeScribe<::ReliquarySerializationTestsFixture::BasicShardWithDifferentInputHandle, BinaryArchive>
    {
    public:
        static std::vector<Type> InputTypes(const ArchiveT& archive)
        {
            return { "ReliquarySerializationTestsBasicShard" };
        }
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::OtherShard, BinaryArchive> final
        : public ArcaCompositeScribe<::ReliquarySerializationTestsFixture::OtherShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::TypedClosedRelic, BinaryArchive> final
        : public ArcaCompositeScribe<::ReliquarySerializationTestsFixture::TypedClosedRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myInt);
        }
    };

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::TypedOpenRelic, BinaryArchive> final
        : public ArcaCompositeScribe<::ReliquarySerializationTestsFixture::TypedOpenRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myInt);
        }
    };

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::GlobalRelic, BinaryArchive> final
        : public ArcaCompositeScribe<::ReliquarySerializationTestsFixture::GlobalRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myInt);
        }
    };

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::BasicCurator, BinaryArchive> final :
        public ArcaCompositeScribe<::ReliquarySerializationTestsFixture::BasicCurator, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myInt);
        }
    };

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::BasicShardNullInscription, BinaryArchive> final
        : public ArcaNullScribe<::ReliquarySerializationTestsFixture::BasicShardNullInscription, BinaryArchive>
    {};

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::OtherShardNullInscription, BinaryArchive> final
        : public ArcaNullScribe<::ReliquarySerializationTestsFixture::OtherShardNullInscription, BinaryArchive>
    {};

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::TypedClosedRelicNullInscription, BinaryArchive> final
        : public ArcaNullScribe<::ReliquarySerializationTestsFixture::TypedClosedRelicNullInscription, BinaryArchive>
    {};

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::TypedOpenRelicNullInscription, BinaryArchive> final
        : public ArcaNullScribe<::ReliquarySerializationTestsFixture::TypedOpenRelicNullInscription, BinaryArchive>
    {};

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::MovableOnlyRelic, BinaryArchive> final
        : public ArcaCompositeScribe<::ReliquarySerializationTestsFixture::MovableOnlyRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::GlobalRelicNullInscription, BinaryArchive> final
        : public ArcaNullScribe<::ReliquarySerializationTestsFixture::GlobalRelicNullInscription, BinaryArchive>
    {};

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::BasicCuratorNullInscription, BinaryArchive> final
        : public ArcaNullScribe<::ReliquarySerializationTestsFixture::BasicCuratorNullInscription, BinaryArchive>
    {};
}