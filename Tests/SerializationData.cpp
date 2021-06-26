#include "SerializationData.h"

SerializationData::BasicShard::BasicShard(std::string myValue) : myValue(myValue)
{}

SerializationData::PreferentialSerializationConstructorShard::PreferentialSerializationConstructorShard(Serialization) :
    calledPreferential(true)
{}

SerializationData::OtherShard::OtherShard(int myValue) : myValue(myValue)
{}

SerializationData::LocalRelic::LocalRelic(RelicInit init, int myInt)
{
    basicShard = init.Create<BasicShard>();
}

SerializationData::LocalRelic::LocalRelic(RelicInit init, Serialization)
{
    basicShard = init.Find<BasicShard>();
}

SerializationData::GlobalRelic::GlobalRelic(RelicInit init)
{
    basicShard = init.Create<BasicShard>();
}

SerializationData::GlobalRelic::GlobalRelic(RelicInit init, int myInt, std::string shardData)
{
    basicShard = init.Create<BasicShard>(std::move(shardData));
}

SerializationData::GlobalRelic::GlobalRelic(RelicInit init, Serialization)
{
    basicShard = init.Find<BasicShard>();
}

SerializationData::NonDefaultConstructorRelic::NonDefaultConstructorRelic(
    RelicInit init, int myInt)
{}

SerializationData::NonDefaultConstructorRelic::NonDefaultConstructorRelic(RelicInit init, Serialization)
{}

SerializationData::BasicShardNullInscription::BasicShardNullInscription(std::string myValue) :
    myValue(std::move(myValue))
{}

SerializationData::BasicShardNullInscription::BasicShardNullInscription(Serialization)
{}

SerializationData::OtherShardNullInscription::OtherShardNullInscription(int myValue) :
    myValue(myValue)
{}

SerializationData::MovableOnlyRelic::MovableOnlyRelic(
    RelicInit init, int myInt, std::string shardData)
    :
    myValue(myInt)
{
    basicShard = init.Create<BasicShard>(std::move(shardData));
}

SerializationData::MovableOnlyRelic::MovableOnlyRelic(RelicInit init, Serialization)
{
    basicShard = init.Find<BasicShard>();
}