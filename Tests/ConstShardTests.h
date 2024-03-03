#pragma once

#include "ReliquaryFixture.h"

using namespace Arca;

class ConstShardTestsFixture : public ReliquaryFixture
{
public:
    class Shard;
    class Relic;
};

namespace Arca
{
    template<>
    struct Traits<::ConstShardTestsFixture::Shard>
    {
        static constexpr ObjectType objectType = ObjectType::Shard;
        static const inline TypeName typeName = "ConstShardTestsShard";
    };

    template<>
    struct Traits<::ConstShardTestsFixture::Relic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "ConstShardTestsRelic";
    };
}

class ConstShardTestsFixture::Shard
{
public:
    int value = 100;
public:
    explicit Shard() = default;
    explicit Shard(int value);
};

class ConstShardTestsFixture::Relic final
{
public:
    Index<const Shard> shard;

    explicit Relic(RelicInit init);
};

namespace Inscription
{
    template<>
    class Scribe<ConstShardTestsFixture::Shard> final
    {
    public:
        using ObjectT = ConstShardTestsFixture::Shard;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {
            archive("value", object.value);
        }
    };

    template<class Archive>
    struct ScribeTraits<ConstShardTestsFixture::Shard, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<ConstShardTestsFixture::Shard>;
    };

    template<class Archive>
    struct ScribeTraits<ConstShardTestsFixture::Relic, Archive> final
    {
        using Category = ArcaNullScribeCategory<ConstShardTestsFixture::Relic>;
    };
}