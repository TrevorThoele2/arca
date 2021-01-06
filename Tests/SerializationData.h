#pragma once

#include <Arca/Shard.h>
#include <Arca/Relic.h>
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
    class PreferentialSerializationConstructorRelic1;
    class PreferentialSerializationConstructorRelic2;
    class PreferentialSerializationConstructorRelic3;
    class PreferentialSerializationConstructorRelic4;

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
        static TypeName TypeName() { return "SerializationData::BasicShard"; }
    };

    template<>
    struct Traits<SerializationData::BasicShardWithDifferentInputHandle>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static TypeName TypeName() { return "SerializationData::BasicShardWithDifferentInputHandle"; }
    };

    template<>
    struct Traits<SerializationData::PreferentialSerializationConstructorShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static TypeName TypeName() { return "SerializationData::PreferentialSerializationConstructorShard"; }
    };

    template<>
    struct Traits<SerializationData::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static TypeName TypeName() { return "SerializationData::OtherShard"; }
    };

    template<>
    struct Traits<SerializationData::TypedClosedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "SerializationData::TypedClosedRelic"; }
    };

    template<>
    struct Traits<SerializationData::TypedOpenRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "SerializationData::TypedOpenRelic"; }
        static const Openness openness = Openness::Open;
    };

    template<>
    struct Traits<SerializationData::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "SerializationData::GlobalRelic"; }
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<SerializationData::NonDefaultConstructorRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "SerializationData::NonDefaultConstructorRelic"; }
    };

    template<>
    struct Traits<SerializationData::PreferentialSerializationConstructorRelic1>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "SerializationData::RelicPreferentialSerializationConstructor1"; }
    };

    template<>
    struct Traits<SerializationData::PreferentialSerializationConstructorRelic2>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "SerializationData::RelicPreferentialSerializationConstructor2"; }
    };

    template<>
    struct Traits<SerializationData::PreferentialSerializationConstructorRelic3>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "SerializationData::RelicPreferentialSerializationConstructor3"; }
    };

    template<>
    struct Traits<SerializationData::PreferentialSerializationConstructorRelic4>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "SerializationData::RelicPreferentialSerializationConstructor4"; }
    };

    template<>
    struct Traits<SerializationData::BasicCurator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static TypeName TypeName() { return "SerializationData::BasicCurator"; }
    };

    template<>
    struct Traits<SerializationData::BasicSignal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static TypeName TypeName() { return "SerializationData::BasicSignal"; }
    };

    template<>
    struct Traits<SerializationData::BasicShardNullInscription>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static TypeName TypeName() { return "SerializationData::BasicShardNullInscription"; }
    };

    template<>
    struct Traits<SerializationData::OtherShardNullInscription>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static TypeName TypeName() { return "SerializationData::OtherShardNullInscription"; }
    };

    template<class ShardT>
    struct Traits<SerializationData::TypedClosedRelicNullInscription<ShardT>>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "SerializationData::TypedClosedRelicNullInscription<" + TypeFor<ShardT>().name + ">"; }
    };

    template<class ShardT>
    struct Traits<SerializationData::TypedOpenRelicNullInscription<ShardT>>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "SerializationData::TypedOpenRelicNullInscription<" + TypeFor<ShardT>().name + ">"; }
        static const Openness openness = Openness::Open;
    };

    template<class ShardT>
    struct Traits<SerializationData::GlobalRelicNullInscription<ShardT>>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "SerializationData::GlobalRelicNullInscription<" + TypeFor<ShardT>().name + ">"; }
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<SerializationData::MovableOnlyRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "SerializationData::MovableOnlyRelic"; }
    };

    template<>
    struct Traits<SerializationData::BasicCuratorNullInscription>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static TypeName TypeName() { return "SerializationData::BasicCuratorNullInscription"; }
    };

    template<>
    struct Traits<SerializationData::BasicSignalNullInscription>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static TypeName TypeName() { return "SerializationData::BasicSignalNullInscription"; }
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

class SerializationData::TypedClosedRelic final
{
public:
    int myInt = 0;
    Index<BasicShard> basicShard;
public:
    TypedClosedRelic(RelicInit init, int myInt);
    TypedClosedRelic(RelicInit init, Serialization);
};

class SerializationData::TypedOpenRelic final
{
public:
    int myInt = 0;
    Index<BasicShard> basicShard;
public:
    TypedOpenRelic(RelicInit init, int myInt);
    TypedOpenRelic(RelicInit init, Serialization);
};

class SerializationData::GlobalRelic final
{
public:
    int myInt = 0;
    Index<BasicShard> basicShard;
public:
    explicit GlobalRelic(RelicInit init);
    GlobalRelic(RelicInit init, int myInt, std::string shardData);
    GlobalRelic(RelicInit init, Serialization);
};

class SerializationData::NonDefaultConstructorRelic final
{
public:
    int myInt = 0;
public:
    NonDefaultConstructorRelic(RelicInit init, int myInt);
    NonDefaultConstructorRelic(RelicInit init, Serialization);
};

class SerializationData::PreferentialSerializationConstructorRelic1 final
{
public:
    bool calledPreferential = false;
public:
    explicit PreferentialSerializationConstructorRelic1(RelicInit, Serialization) :
        calledPreferential(true)
    {}

    explicit PreferentialSerializationConstructorRelic1(Serialization)
    {}

    explicit PreferentialSerializationConstructorRelic1(RelicInit)
    {}

    explicit PreferentialSerializationConstructorRelic1() = default;
};

class SerializationData::PreferentialSerializationConstructorRelic2 final
{
public:
    bool calledPreferential = false;
public:
    explicit PreferentialSerializationConstructorRelic2(Serialization) :
        calledPreferential(true)
    {}

    explicit PreferentialSerializationConstructorRelic2(RelicInit)
    {}

    explicit PreferentialSerializationConstructorRelic2() = default;
};

class SerializationData::PreferentialSerializationConstructorRelic3 final
{
public:
    bool calledPreferential = false;
public:
    explicit PreferentialSerializationConstructorRelic3(RelicInit) :
        calledPreferential(true)
    {}

    explicit PreferentialSerializationConstructorRelic3() = default;
};

class SerializationData::PreferentialSerializationConstructorRelic4 final
{
public:
    bool calledPreferential = false;
public:
    explicit PreferentialSerializationConstructorRelic4() :
        calledPreferential(true)
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
class SerializationData::TypedClosedRelicNullInscription final
{
public:
    int myInt = 0;
    Index<ShardT> shard;
public:
    template<class... ShardArgs>
    TypedClosedRelicNullInscription(RelicInit init, ShardArgs&& ... shardArgs);
    template<class... ShardArgs>
    TypedClosedRelicNullInscription(RelicInit init, int myInt, ShardArgs&& ... shardArgs);
    TypedClosedRelicNullInscription(RelicInit init, Serialization);
};

template<class ShardT>
template<class... ShardArgs>
SerializationData::TypedClosedRelicNullInscription<ShardT>::TypedClosedRelicNullInscription(
    RelicInit init,
    ShardArgs&& ... shardArgs)
{
    shard = init.template Create<ShardT>(std::forward<ShardArgs>(shardArgs)...);
}

template<class ShardT>
template<class... ShardArgs>
SerializationData::TypedClosedRelicNullInscription<ShardT>::TypedClosedRelicNullInscription(
    RelicInit init,
    int myInt,
    ShardArgs&& ... shardArgs)
    :
    myInt(myInt)
{
    shard = init.template Create<ShardT>(std::forward<ShardArgs>(shardArgs)...);
}

template<class ShardT>
SerializationData::TypedClosedRelicNullInscription<ShardT>::TypedClosedRelicNullInscription(
    RelicInit init, Serialization)
{}

template<class ShardT>
class SerializationData::TypedOpenRelicNullInscription final
{
public:
    int myInt = 0;
    Index<ShardT> shard;
public:
    template<class... ShardArgs>
    TypedOpenRelicNullInscription(RelicInit init, ShardArgs&& ... shardArgs);
    template<class... ShardArgs>
    TypedOpenRelicNullInscription(RelicInit init, int myInt, ShardArgs&& ... shardArgs);
    TypedOpenRelicNullInscription(RelicInit init, Serialization);
};

template<class ShardT>
template<class... ShardArgs>
SerializationData::TypedOpenRelicNullInscription<ShardT>::TypedOpenRelicNullInscription(
    RelicInit init,
    ShardArgs&& ... shardArgs)
{
    shard = init.template Create<ShardT>(std::forward<ShardArgs>(shardArgs)...);
}

template<class ShardT>
template<class... ShardArgs>
SerializationData::TypedOpenRelicNullInscription<ShardT>::TypedOpenRelicNullInscription(
    RelicInit init,
    int myInt,
    ShardArgs&& ... shardArgs)
    :
    myInt(myInt)
{
    shard = init.template Create<ShardT>(std::forward<ShardArgs>(shardArgs)...);
}

template<class ShardT>
SerializationData::TypedOpenRelicNullInscription<ShardT>::TypedOpenRelicNullInscription(
    RelicInit init, Serialization)
{}

template<class ShardT>
class SerializationData::GlobalRelicNullInscription final
{
public:
    int myInt = 0;
    Index<ShardT> shard;
public:
    template<class... ShardArgs>
    GlobalRelicNullInscription(RelicInit init, ShardArgs&& ... shardArgs);
    template<class... ShardArgs>
    GlobalRelicNullInscription(RelicInit init, int myInt, ShardArgs&& ... shardArgs);
};

template<class ShardT>
template<class... ShardArgs>
SerializationData::GlobalRelicNullInscription<ShardT>::GlobalRelicNullInscription(
    RelicInit init,
    ShardArgs&& ... shardArgs)
{
    shard = init.template Create<ShardT>(std::forward<ShardArgs>(shardArgs)...);
}

template<class ShardT>
template<class... ShardArgs>
SerializationData::GlobalRelicNullInscription<ShardT>::GlobalRelicNullInscription(
    RelicInit init,
    int myInt,
    ShardArgs&& ... shardArgs)
    :
    myInt(myInt)
{
    shard = init.template Create<ShardT>(std::forward<ShardArgs>(shardArgs)...);
}

class SerializationData::MovableOnlyRelic final
{
public:
    Index<BasicShard> basicShard;

    int myValue = 0;
public:
    MovableOnlyRelic(RelicInit init, int myInt, std::string shardData);
    MovableOnlyRelic(RelicInit init, Serialization);
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
    class Scribe<SerializationData::PreferentialSerializationConstructorRelic1> final
    {
    public:
        using ObjectT = SerializationData::PreferentialSerializationConstructorRelic1;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {}
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::PreferentialSerializationConstructorRelic1, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<SerializationData::PreferentialSerializationConstructorRelic1>;
    };

    template<>
    class Scribe<SerializationData::PreferentialSerializationConstructorRelic2> final
    {
    public:
        using ObjectT = SerializationData::PreferentialSerializationConstructorRelic2;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {}
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::PreferentialSerializationConstructorRelic2, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<SerializationData::PreferentialSerializationConstructorRelic2>;
    };

    template<>
    class Scribe<SerializationData::PreferentialSerializationConstructorRelic3> final
    {
    public:
        using ObjectT = SerializationData::PreferentialSerializationConstructorRelic3;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {}
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::PreferentialSerializationConstructorRelic3, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<SerializationData::PreferentialSerializationConstructorRelic3>;
    };

    template<>
    class Scribe<SerializationData::PreferentialSerializationConstructorRelic4> final
    {
    public:
        using ObjectT = SerializationData::PreferentialSerializationConstructorRelic4;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {}
    };

    template<class Archive>
    struct ScribeTraits<SerializationData::PreferentialSerializationConstructorRelic4, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<SerializationData::PreferentialSerializationConstructorRelic4>;
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