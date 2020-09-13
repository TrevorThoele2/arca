#pragma once

#include "ReliquaryFixture.h"

#include <Arca/ClosedTypedRelic.h>

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
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ConstShardTestsShard";
    };

    template<>
    struct Traits<::ConstShardTestsFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ConstShardTestsRelic";
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

class ConstShardTestsFixture::Relic final : public ClosedTypedRelic<Relic>
{
public:
    Index<const Shard> shard;

    explicit Relic(Init init);
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