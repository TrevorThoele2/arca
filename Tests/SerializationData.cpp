#include "SerializationData.h"

SerializationData::BasicShard::BasicShard(std::string myValue) : myValue(myValue)
{}

SerializationData::PreferentialSerializationConstructorShard::PreferentialSerializationConstructorShard(Serialization) :
    calledPreferential(true)
{}

SerializationData::OtherShard::OtherShard(int myValue) : myValue(myValue)
{}

SerializationData::TypedClosedRelic::TypedClosedRelic(Init init, int myInt) :
    ClosedTypedRelic(init), myInt(myInt)
{
    basicShard = Create<BasicShard>();
}

SerializationData::TypedClosedRelic::TypedClosedRelic(Init init, Serialization) :
    ClosedTypedRelic(init)
{
    basicShard = Find<BasicShard>();
}

SerializationData::TypedOpenRelic::TypedOpenRelic(Init init, int myInt) :
    OpenTypedRelic(init), myInt(myInt)
{
    basicShard = Create<BasicShard>();
}

SerializationData::TypedOpenRelic::TypedOpenRelic(Init init, Serialization) :
    OpenTypedRelic(init)
{
    basicShard = Find<BasicShard>();
}

SerializationData::GlobalRelic::GlobalRelic(Init init) :
    ClosedTypedRelic(init)
{
    basicShard = Create<BasicShard>();
}

SerializationData::GlobalRelic::GlobalRelic(Init init, int myInt, std::string shardData) :
    ClosedTypedRelic(init), myInt(myInt)
{
    basicShard = Create<BasicShard>(std::move(shardData));
}

SerializationData::GlobalRelic::GlobalRelic(Init init, Serialization) :
    ClosedTypedRelic(init)
{
    basicShard = Find<BasicShard>();
}

SerializationData::NonDefaultConstructorRelic::NonDefaultConstructorRelic(
    Init init, int myInt)
    : ClosedTypedRelic(init), myInt(myInt)
{}

SerializationData::BasicShardNullInscription::BasicShardNullInscription(std::string myValue)
    : myValue(std::move(myValue))
{}

SerializationData::OtherShardNullInscription::OtherShardNullInscription(int myValue)
    : myValue(myValue)
{}

SerializationData::TypedClosedRelicNullInscription::TypedClosedRelicNullInscription(
    Init init)
    :
    ClosedTypedRelic(init)
{
    basicShard = Create<BasicShardNullInscription>();
}

SerializationData::TypedClosedRelicNullInscription::TypedClosedRelicNullInscription(
    Init init, int myInt)
    :
    ClosedTypedRelic(init), myInt(myInt)
{
    basicShard = Create<BasicShardNullInscription>();
}

SerializationData::TypedOpenRelicNullInscription::TypedOpenRelicNullInscription(
    Init init)
    :
    OpenTypedRelic(init)
{
    basicShard = Create<BasicShardNullInscription>();
}

SerializationData::TypedOpenRelicNullInscription::TypedOpenRelicNullInscription(
    Init init, int myInt)
    :
    OpenTypedRelic(init), myInt(myInt)
{
    basicShard = Create<BasicShardNullInscription>();
}

SerializationData::GlobalRelicNullInscription::GlobalRelicNullInscription(
    Init init)
    :
    ClosedTypedRelic(init)
{
    basicShard = Create<BasicShardNullInscription>();
}

SerializationData::GlobalRelicNullInscription::GlobalRelicNullInscription(
    Init init, int myInt, std::string shardData)
    :
    ClosedTypedRelic(init), myInt(myInt)
{
    basicShard = Create<BasicShardNullInscription>(std::move(shardData));
}

SerializationData::MovableOnlyRelic::MovableOnlyRelic(
    Init init, int myInt, std::string shardData)
    :
    ClosedTypedRelic(init), myValue(myInt)
{
    basicShard = Create<BasicShard>(std::move(shardData));
}

SerializationData::MovableOnlyRelic::MovableOnlyRelic(Init init, Serialization) :
    ClosedTypedRelic(init)
{
    basicShard = Find<BasicShard>();
}