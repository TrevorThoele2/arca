#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelic.h>
#include <Arca/OpenTypedRelic.h>

#include <Inscription/BinaryArchive.h>

using namespace Arca;

class ReliquarySerializationTestsFixture : public GeneralFixture
{
public:
    class BasicShard;
    class BasicShardWithDifferentInputHandle;
    class OtherShard;
    class PreferentialSerializationConstructorShard;

    class TypedClosedRelic;
    class TypedOpenRelic;
    class GlobalRelic;
    class NonDefaultConstructorRelic;
    class PreferentialSerializationConstructorRelic;

    class BasicCurator;

    struct BasicSignal;

    class BasicShardNullInscription;
    class OtherShardNullInscription;

    class TypedClosedRelicNullInscription;
    class TypedOpenRelicNullInscription;
    class GlobalRelicNullInscription;
    class MovableOnlyRelic;

    class BasicCuratorNullInscription;

    struct BasicSignalNullInscription;
};

namespace Arca
{
    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ReliquarySerializationTests_BasicShard";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicShardWithDifferentInputHandle>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ReliquarySerializationTests_BasicShardWithDifferentInputHandle";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::PreferentialSerializationConstructorShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ReliquarySerializationTests_PreferentialSerializationConstructorShard";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ReliquarySerializationTests_OtherShard";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::TypedClosedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquarySerializationTests_TypedClosedRelic";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::TypedOpenRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquarySerializationTests_TypedOpenRelic";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquarySerializationTests_GlobalRelic";
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::NonDefaultConstructorRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquarySerializationTests_NonDefaultConstructorRelic";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::PreferentialSerializationConstructorRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquarySerializationTests_RelicPreferentialSerializationConstructor";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicCurator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "ReliquarySerializationTests_BasicCurator";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicSignal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "ReliquarySerializationTests_BasicSignal";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicShardNullInscription>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ReliquarySerializationTests_BasicShardNullInscription";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::OtherShardNullInscription>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ReliquarySerializationTests_OtherShardNullInscription";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::TypedClosedRelicNullInscription>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquarySerializationTests_TypedClosedRelicNullInscription";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::TypedOpenRelicNullInscription>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquarySerializationTests_TypedOpenRelicNullInscription";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::GlobalRelicNullInscription>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquarySerializationTests_GlobalRelicNullInscription";
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::MovableOnlyRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquarySerializationTests_MovableOnlyRelic";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicCuratorNullInscription>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "ReliquarySerializationTests_BasicCuratorNullInscription";
    };

    template<>
    struct Traits<::ReliquarySerializationTestsFixture::BasicSignalNullInscription>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "ReliquarySerializationTests_BasicSignalNullInscription";
    };
}

class ReliquarySerializationTestsFixture::BasicShard
{
public:
    std::string myValue;
public:
    BasicShard() = default;
    explicit BasicShard(std::string myValue);
};

class ReliquarySerializationTestsFixture::BasicShardWithDifferentInputHandle
{
public:
    std::string myValue;
public:
    BasicShardWithDifferentInputHandle() = default;
    explicit BasicShardWithDifferentInputHandle(int myValue);
};

class ReliquarySerializationTestsFixture::PreferentialSerializationConstructorShard
{
public:
    bool calledPreferential = false;
public:
    PreferentialSerializationConstructorShard() = default;
    explicit PreferentialSerializationConstructorShard(Serialization);
};

class ReliquarySerializationTestsFixture::OtherShard
{
public:
    int myValue;
public:
    OtherShard() = default;
    explicit OtherShard(int myValue);
};

class ReliquarySerializationTestsFixture::TypedClosedRelic final : public ClosedTypedRelic<TypedClosedRelic>
{
public:
    int myInt = 0;
    Index<BasicShard> basicShard;
public:
    TypedClosedRelic(Init init, int myInt);
    TypedClosedRelic(Init init, Serialization);
};

class ReliquarySerializationTestsFixture::TypedOpenRelic final : public OpenTypedRelic<TypedOpenRelic>
{
public:
    int myInt = 0;
    Index<BasicShard> basicShard;
public:
    TypedOpenRelic(Init init, int myInt);
    TypedOpenRelic(Init init, Serialization);
};

class ReliquarySerializationTestsFixture::GlobalRelic final : public ClosedTypedRelic<GlobalRelic>
{
public:
    int myInt = 0;
    Index<BasicShard> basicShard;
public:
    explicit GlobalRelic(Init init);
    GlobalRelic(Init init, int myInt, std::string shardData);
};

class ReliquarySerializationTestsFixture::NonDefaultConstructorRelic final
    : public ClosedTypedRelic<NonDefaultConstructorRelic>
{
public:
    int myInt = 0;
public:
    NonDefaultConstructorRelic(Init init, int myInt);
};

class ReliquarySerializationTestsFixture::PreferentialSerializationConstructorRelic final :
    public ClosedTypedRelic<PreferentialSerializationConstructorRelic>
{
public:
    bool calledPreferential = false;
public:
    explicit PreferentialSerializationConstructorRelic(Init init) :
        ClosedTypedRelic(init)
    {}

    explicit PreferentialSerializationConstructorRelic(Init init, Serialization) :
        ClosedTypedRelic(init), calledPreferential(true)
    {}
};

class ReliquarySerializationTestsFixture::BasicCurator final : public Curator
{
public:
    int myInt = 0;

    using Curator::Curator;
};

struct ReliquarySerializationTestsFixture::BasicSignal
{};

class ReliquarySerializationTestsFixture::BasicShardNullInscription
{
public:
    std::string myValue;
public:
    BasicShardNullInscription() = default;
    explicit BasicShardNullInscription(std::string myValue);
};

class ReliquarySerializationTestsFixture::OtherShardNullInscription
{
public:
    int myValue;
public:
    explicit OtherShardNullInscription(int myValue = 0);
};

class ReliquarySerializationTestsFixture::TypedClosedRelicNullInscription final :
    public ClosedTypedRelic<TypedClosedRelicNullInscription>
{
public:
    int myInt = 0;
    Index<BasicShardNullInscription> basicShard;
public:
    explicit TypedClosedRelicNullInscription(Init init);
    TypedClosedRelicNullInscription(Init init, int myInt);
};

class ReliquarySerializationTestsFixture::TypedOpenRelicNullInscription final :
    public OpenTypedRelic<TypedOpenRelicNullInscription>
{
public:
    int myInt = 0;
    Index<BasicShardNullInscription> basicShard;
public:
    explicit TypedOpenRelicNullInscription(Init init);
    TypedOpenRelicNullInscription(Init init, int myInt);
};

class ReliquarySerializationTestsFixture::GlobalRelicNullInscription final :
    public ClosedTypedRelic<GlobalRelicNullInscription>
{
public:
    int myInt = 0;
    Index<BasicShardNullInscription> basicShard;
public:
    explicit GlobalRelicNullInscription(Init init);
    GlobalRelicNullInscription(Init init, int myInt, std::string shardData);
};

class ReliquarySerializationTestsFixture::MovableOnlyRelic final : public ClosedTypedRelic<MovableOnlyRelic>
{
public:
    Index<BasicShard> basicShard;

    int myValue = 0;
public:
    MovableOnlyRelic(Init init, int myInt, std::string shardData);
    MovableOnlyRelic(Init init, Serialization);
    MovableOnlyRelic(const MovableOnlyRelic& arg) = delete;
    MovableOnlyRelic(MovableOnlyRelic&& arg) noexcept = default;
    MovableOnlyRelic& operator=(const MovableOnlyRelic& arg) = delete;
    MovableOnlyRelic& operator=(MovableOnlyRelic&& arg) noexcept = default;
};

class ReliquarySerializationTestsFixture::BasicCuratorNullInscription final : public Curator
{
public:
    int myInt = 0;

    using Curator::Curator;
};

struct ReliquarySerializationTestsFixture::BasicSignalNullInscription
{};

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
            return { TypeFor<ReliquarySerializationTestsFixture::BasicShard>().name };
        }
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::PreferentialSerializationConstructorShard, BinaryArchive> final
        : public ArcaCompositeScribe<::ReliquarySerializationTestsFixture::PreferentialSerializationConstructorShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
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
    class Scribe<::ReliquarySerializationTestsFixture::NonDefaultConstructorRelic, BinaryArchive> final
        : public ArcaNullScribe<::ReliquarySerializationTestsFixture::NonDefaultConstructorRelic, BinaryArchive>
    {};

    template<>
    class Scribe<::ReliquarySerializationTestsFixture::PreferentialSerializationConstructorRelic, BinaryArchive> final
        : public ArcaCompositeScribe<::ReliquarySerializationTestsFixture::PreferentialSerializationConstructorRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {}
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