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

    template<class ShardT>
    class TypedClosedRelicNullInscription;
    template<class ShardT>
    class TypedOpenRelicNullInscription;
    template<class ShardT>
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

    template<class ShardT>
    struct Traits<SerializationData::TypedClosedRelicNullInscription<ShardT>>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "SerializationData_TypedClosedRelicNullInscription<" + TypeFor<ShardT>().name + ">";
    };

    template<class ShardT>
    struct Traits<SerializationData::TypedOpenRelicNullInscription<ShardT>>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "SerializationData_TypedOpenRelicNullInscription<" + TypeFor<ShardT>().name + ">";
    };

    template<class ShardT>
    struct Traits<SerializationData::GlobalRelicNullInscription<ShardT>>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "SerializationData_GlobalRelicNullInscription<" + TypeFor<ShardT>().name + ">";
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
    NonDefaultConstructorRelic(Init init, Serialization);
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
    BasicShardNullInscription(Serialization);
};

class SerializationData::OtherShardNullInscription
{
public:
    int myValue;
public:
    explicit OtherShardNullInscription(int myValue = 0);
};

template<class ShardT>
class SerializationData::TypedClosedRelicNullInscription final :
    public ClosedTypedRelic<TypedClosedRelicNullInscription<ShardT>>
{
private:
    using BaseT = ClosedTypedRelic<TypedClosedRelicNullInscription<ShardT>>;
public:
    using Init = typename BaseT::Init;
protected:
    using BaseT::Create;
public:
    int myInt = 0;
    Index<ShardT> shard;
public:
    template<class... ShardArgs>
    TypedClosedRelicNullInscription(Init init, ShardArgs&& ... shardArgs);
    template<class... ShardArgs>
    TypedClosedRelicNullInscription(Init init, int myInt, ShardArgs&& ... shardArgs);
    TypedClosedRelicNullInscription(Init init, Serialization);
};

template<class ShardT>
template<class... ShardArgs>
SerializationData::TypedClosedRelicNullInscription<ShardT>::TypedClosedRelicNullInscription(
    Init init,
    ShardArgs&& ... shardArgs)
    :
    ClosedTypedRelic<TypedClosedRelicNullInscription<ShardT>>(init)
{
    shard = this->template Create<ShardT>(std::forward<ShardArgs>(shardArgs)...);
}

template<class ShardT>
template<class... ShardArgs>
SerializationData::TypedClosedRelicNullInscription<ShardT>::TypedClosedRelicNullInscription(
    Init init,
    int myInt,
    ShardArgs&& ... shardArgs)
    :
    ClosedTypedRelic<TypedClosedRelicNullInscription<ShardT>>(init),
    myInt(myInt)
{
    shard = this->template Create<ShardT>(std::forward<ShardArgs>(shardArgs)...);
}

template<class ShardT>
SerializationData::TypedClosedRelicNullInscription<ShardT>::TypedClosedRelicNullInscription(Init init, Serialization) :
    ClosedTypedRelic<TypedClosedRelicNullInscription<ShardT>>(init)
{}

template<class ShardT>
class SerializationData::TypedOpenRelicNullInscription final :
    public OpenTypedRelic<TypedOpenRelicNullInscription<ShardT>>
{
private:
    using BaseT = OpenTypedRelic<TypedOpenRelicNullInscription<ShardT>>;
public:
    using Init = typename BaseT::Init;
public:
    using BaseT::Create;
public:
    int myInt = 0;
    Index<ShardT> shard;
public:
    template<class... ShardArgs>
    TypedOpenRelicNullInscription(Init init, ShardArgs&& ... shardArgs);
    template<class... ShardArgs>
    TypedOpenRelicNullInscription(Init init, int myInt, ShardArgs&& ... shardArgs);
    TypedOpenRelicNullInscription(Init init, Serialization);
};

template<class ShardT>
template<class... ShardArgs>
SerializationData::TypedOpenRelicNullInscription<ShardT>::TypedOpenRelicNullInscription(
    Init init,
    ShardArgs&& ... shardArgs)
    :
    OpenTypedRelic<TypedOpenRelicNullInscription<ShardT>>(init)
{
    shard = this->template Create<ShardT>(std::forward<ShardArgs>(shardArgs)...);
}

template<class ShardT>
template<class... ShardArgs>
SerializationData::TypedOpenRelicNullInscription<ShardT>::TypedOpenRelicNullInscription(
    Init init,
    int myInt,
    ShardArgs&& ... shardArgs)
    :
    OpenTypedRelic<TypedOpenRelicNullInscription<ShardT>>(init),
    myInt(myInt)
{
    shard = this->template Create<ShardT>(std::forward<ShardArgs>(shardArgs)...);
}

template<class ShardT>
SerializationData::TypedOpenRelicNullInscription<ShardT>::TypedOpenRelicNullInscription(Init init, Serialization) :
    OpenTypedRelic<TypedOpenRelicNullInscription<ShardT>>(init)
{}

template<class ShardT>
class SerializationData::GlobalRelicNullInscription final :
    public ClosedTypedRelic<GlobalRelicNullInscription<ShardT>>
{
private:
    using BaseT = ClosedTypedRelic<GlobalRelicNullInscription<ShardT>>;
public:
    using Init = typename BaseT::Init;
protected:
    using BaseT::Create;
public:
    int myInt = 0;
    Index<ShardT> shard;
public:
    template<class... ShardArgs>
    GlobalRelicNullInscription(Init init, ShardArgs&& ... shardArgs);
    template<class... ShardArgs>
    GlobalRelicNullInscription(Init init, int myInt, ShardArgs&& ... shardArgs);
};

template<class ShardT>
template<class... ShardArgs>
SerializationData::GlobalRelicNullInscription<ShardT>::GlobalRelicNullInscription(
    Init init,
    ShardArgs&& ... shardArgs)
    :
    ClosedTypedRelic<GlobalRelicNullInscription<ShardT>>(init)
{
    shard = this->template Create<ShardT>(std::forward<ShardArgs>(shardArgs)...);
}

template<class ShardT>
template<class... ShardArgs>
SerializationData::GlobalRelicNullInscription<ShardT>::GlobalRelicNullInscription(
    Init init,
    int myInt,
    ShardArgs&& ... shardArgs)
    :
    ClosedTypedRelic<GlobalRelicNullInscription<ShardT>>(init),
    myInt(myInt)
{
    shard = this->template Create<ShardT>(std::forward<ShardArgs>(shardArgs)...);
}

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

    template<class ShardT, class Archive>
    struct ScribeTraits<SerializationData::TypedClosedRelicNullInscription<ShardT>, Archive> final
    {
        using Category = ArcaNullScribeCategory<SerializationData::TypedClosedRelicNullInscription<ShardT>>;
    };

    template<class ShardT, class Archive>
    struct ScribeTraits<SerializationData::TypedOpenRelicNullInscription<ShardT>, Archive> final
    {
        using Category = ArcaNullScribeCategory<SerializationData::TypedOpenRelicNullInscription<ShardT>>;
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

    template<class ShardT, class Archive>
    struct ScribeTraits<SerializationData::GlobalRelicNullInscription<ShardT>, Archive> final
    {
        using Category = ArcaNullScribeCategory<SerializationData::GlobalRelicNullInscription<ShardT>>;
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::BasicCuratorNullInscription, Archive> final
    {
        using Category = ArcaNullScribeCategory<SerializationData::BasicCuratorNullInscription>;
    };
}