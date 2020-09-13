#pragma once

#include <Arca/Shard.h>
#include <Arca/ClosedTypedRelic.h>
#include <Arca/OpenTypedRelic.h>
#include <Arca/Curator.h>

using namespace Arca;

class SerializationData
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
    struct Traits<SerializationData::BasicShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "SerializationData_BasicShard";
    };

    template<>
    struct Traits<SerializationData::BasicShardWithDifferentInputHandle>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "SerializationData_BasicShardWithDifferentInputHandle";
    };

    template<>
    struct Traits<SerializationData::PreferentialSerializationConstructorShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "SerializationData_PreferentialSerializationConstructorShard";
    };

    template<>
    struct Traits<SerializationData::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "SerializationData_OtherShard";
    };

    template<>
    struct Traits<SerializationData::TypedClosedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "SerializationData_TypedClosedRelic";
    };

    template<>
    struct Traits<SerializationData::TypedOpenRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "SerializationData_TypedOpenRelic";
    };

    template<>
    struct Traits<SerializationData::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "SerializationData_GlobalRelic";
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<SerializationData::NonDefaultConstructorRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "SerializationData_NonDefaultConstructorRelic";
    };

    template<>
    struct Traits<SerializationData::PreferentialSerializationConstructorRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "SerializationData_RelicPreferentialSerializationConstructor";
    };

    template<>
    struct Traits<SerializationData::BasicCurator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "SerializationData_BasicCurator";
    };

    template<>
    struct Traits<SerializationData::BasicSignal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "SerializationData_BasicSignal";
    };

    template<>
    struct Traits<SerializationData::BasicShardNullInscription>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "SerializationData_BasicShardNullInscription";
    };

    template<>
    struct Traits<SerializationData::OtherShardNullInscription>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "SerializationData_OtherShardNullInscription";
    };

    template<>
    struct Traits<SerializationData::TypedClosedRelicNullInscription>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "SerializationData_TypedClosedRelicNullInscription";
    };

    template<>
    struct Traits<SerializationData::TypedOpenRelicNullInscription>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "SerializationData_TypedOpenRelicNullInscription";
    };

    template<>
    struct Traits<SerializationData::GlobalRelicNullInscription>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "SerializationData_GlobalRelicNullInscription";
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<SerializationData::MovableOnlyRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "SerializationData_MovableOnlyRelic";
    };

    template<>
    struct Traits<SerializationData::BasicCuratorNullInscription>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "SerializationData_BasicCuratorNullInscription";
    };

    template<>
    struct Traits<SerializationData::BasicSignalNullInscription>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "SerializationData_BasicSignalNullInscription";
    };
}

class SerializationData::BasicShard
{
public:
    std::string myValue;
public:
    BasicShard() = default;
    explicit BasicShard(std::string myValue);
};

class SerializationData::BasicShardWithDifferentInputHandle
{
public:
    std::string myValue;
public:
    BasicShardWithDifferentInputHandle() = default;
    explicit BasicShardWithDifferentInputHandle(int myValue);
};

class SerializationData::PreferentialSerializationConstructorShard
{
public:
    bool calledPreferential = false;
public:
    PreferentialSerializationConstructorShard() = default;
    explicit PreferentialSerializationConstructorShard(Serialization);
};

class SerializationData::OtherShard
{
public:
    int myValue;
public:
    OtherShard() = default;
    explicit OtherShard(int myValue);
};

class SerializationData::TypedClosedRelic final : public ClosedTypedRelic<TypedClosedRelic>
{
public:
    int myInt = 0;
    Index<BasicShard> basicShard;
public:
    TypedClosedRelic(Init init, int myInt);
    TypedClosedRelic(Init init, Serialization);
};

class SerializationData::TypedOpenRelic final : public OpenTypedRelic<TypedOpenRelic>
{
public:
    int myInt = 0;
    Index<BasicShard> basicShard;
public:
    TypedOpenRelic(Init init, int myInt);
    TypedOpenRelic(Init init, Serialization);
};

class SerializationData::GlobalRelic final : public ClosedTypedRelic<GlobalRelic>
{
public:
    int myInt = 0;
    Index<BasicShard> basicShard;
public:
    explicit GlobalRelic(Init init);
    GlobalRelic(Init init, int myInt, std::string shardData);
    GlobalRelic(Init init, Serialization);
};

class SerializationData::NonDefaultConstructorRelic final
    : public ClosedTypedRelic<NonDefaultConstructorRelic>
{
public:
    int myInt = 0;
public:
    NonDefaultConstructorRelic(Init init, int myInt);
};

class SerializationData::PreferentialSerializationConstructorRelic final :
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

class SerializationData::BasicCurator final : public Curator
{
public:
    int myInt = 0;

    using Curator::Curator;
};

struct SerializationData::BasicSignal
{};

class SerializationData::BasicShardNullInscription
{
public:
    std::string myValue;
public:
    BasicShardNullInscription() = default;
    explicit BasicShardNullInscription(std::string myValue);
};

class SerializationData::OtherShardNullInscription
{
public:
    int myValue;
public:
    explicit OtherShardNullInscription(int myValue = 0);
};

class SerializationData::TypedClosedRelicNullInscription final :
    public ClosedTypedRelic<TypedClosedRelicNullInscription>
{
public:
    int myInt = 0;
    Index<BasicShardNullInscription> basicShard;
public:
    explicit TypedClosedRelicNullInscription(Init init);
    TypedClosedRelicNullInscription(Init init, int myInt);
};

class SerializationData::TypedOpenRelicNullInscription final :
    public OpenTypedRelic<TypedOpenRelicNullInscription>
{
public:
    int myInt = 0;
    Index<BasicShardNullInscription> basicShard;
public:
    explicit TypedOpenRelicNullInscription(Init init);
    TypedOpenRelicNullInscription(Init init, int myInt);
};

class SerializationData::GlobalRelicNullInscription final :
    public ClosedTypedRelic<GlobalRelicNullInscription>
{
public:
    int myInt = 0;
    Index<BasicShardNullInscription> basicShard;
public:
    explicit GlobalRelicNullInscription(Init init);
    GlobalRelicNullInscription(Init init, int myInt, std::string shardData);
};

class SerializationData::MovableOnlyRelic final : public ClosedTypedRelic<MovableOnlyRelic>
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

class SerializationData::BasicCuratorNullInscription final : public Curator
{
public:
    int myInt = 0;

    using Curator::Curator;
};

struct SerializationData::BasicSignalNullInscription
{};

namespace Inscription
{
    template<>
    class Scribe<SerializationData::BasicShard> final
    {
    public:
        using ObjectT = SerializationData::BasicShard;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {
            archive("myValue", object.myValue);
        }
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::BasicShard, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<SerializationData::BasicShard>;
    };

    template<>
    class Scribe<SerializationData::BasicShardWithDifferentInputHandle> final
    {
    public:
        using ObjectT = SerializationData::BasicShardWithDifferentInputHandle;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {
            archive("myValue", object.myValue);
        }

        template<class Archive>
        static std::vector<Type> InputTypes(const Archive& archive)
        {
            return { TypeFor<SerializationData::BasicShard>().name };
        }
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::BasicShardWithDifferentInputHandle, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<SerializationData::BasicShardWithDifferentInputHandle>;
    };

    template<>
    class Scribe<SerializationData::PreferentialSerializationConstructorShard> final
    {
    public:
        using ObjectT = SerializationData::PreferentialSerializationConstructorShard;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {}
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::PreferentialSerializationConstructorShard, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<SerializationData::PreferentialSerializationConstructorShard>;
    };

    template<>
    class Scribe<SerializationData::OtherShard> final
    {
    public:
        using ObjectT = SerializationData::OtherShard;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {
            archive("myValue", object.myValue);
        }
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::OtherShard, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<SerializationData::OtherShard>;
    };

    template<>
    class Scribe<SerializationData::TypedClosedRelic> final
    {
    public:
        using ObjectT = SerializationData::TypedClosedRelic;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {
            archive("myInt", object.myInt);
        }
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::TypedClosedRelic, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<SerializationData::TypedClosedRelic>;
    };

    template<>
    class Scribe<SerializationData::TypedOpenRelic> final
    {
    public:
        using ObjectT = SerializationData::TypedOpenRelic;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {
            archive("myInt", object.myInt);
        }
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::TypedOpenRelic, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<SerializationData::TypedOpenRelic>;
    };

    template<>
    class Scribe<SerializationData::GlobalRelic> final
    {
    public:
        using ObjectT = SerializationData::GlobalRelic;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {
            archive("myInt", object.myInt);
        }
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::GlobalRelic, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<SerializationData::GlobalRelic>;
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::NonDefaultConstructorRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<SerializationData::NonDefaultConstructorRelic>;
    };

    template<>
    class Scribe<SerializationData::PreferentialSerializationConstructorRelic> final
    {
    public:
        using ObjectT = SerializationData::PreferentialSerializationConstructorRelic;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {}
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::PreferentialSerializationConstructorRelic, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<SerializationData::PreferentialSerializationConstructorRelic>;
    };

    template<>
    class Scribe<SerializationData::BasicCurator> final
    {
    public:
        using ObjectT = SerializationData::BasicCurator;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {
            archive("myInt", object.myInt);
        }
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::BasicCurator, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<SerializationData::BasicCurator>;
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::BasicShardNullInscription, Archive> final
    {
        using Category = ArcaNullScribeCategory<SerializationData::BasicShardNullInscription>;
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::OtherShardNullInscription, Archive> final
    {
        using Category = ArcaNullScribeCategory<SerializationData::OtherShardNullInscription>;
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::TypedClosedRelicNullInscription, Archive> final
    {
        using Category = ArcaNullScribeCategory<SerializationData::TypedClosedRelicNullInscription>;
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::TypedOpenRelicNullInscription, Archive> final
    {
        using Category = ArcaNullScribeCategory<SerializationData::TypedOpenRelicNullInscription>;
    };

    template<>
    class Scribe<SerializationData::MovableOnlyRelic> final
    {
    public:
        using ObjectT = SerializationData::MovableOnlyRelic;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {
            archive("myValue", object.myValue);
        }
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::MovableOnlyRelic, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<SerializationData::MovableOnlyRelic>;
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::GlobalRelicNullInscription, Archive> final
    {
        using Category = ArcaNullScribeCategory<SerializationData::GlobalRelicNullInscription>;
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::BasicCuratorNullInscription, Archive> final
    {
        using Category = ArcaNullScribeCategory<SerializationData::BasicCuratorNullInscription>;
    };
}