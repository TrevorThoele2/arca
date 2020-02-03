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

class ConstShardTestsFixture::Shard
{
public:
    int value = 100;
public:
    Shard() = default;
    explicit Shard(int value);
};

class ConstShardTestsFixture::Relic final : public ClosedTypedRelic<Relic>
{
public:
    ShardIndex<const Shard> shard;

    explicit Relic(Init init);
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

namespace Inscription
{
    template<>
    class Scribe<::ConstShardTestsFixture::Shard, BinaryArchive> final
        : public ArcaCompositeScribe<::ConstShardTestsFixture::Shard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.value);
        }
    };

    template<>
    class Scribe<::ConstShardTestsFixture::Relic, BinaryArchive> final
        : public ArcaNullScribe<::ConstShardTestsFixture::Relic, BinaryArchive>
    {};
}