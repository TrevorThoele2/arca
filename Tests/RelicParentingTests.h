#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelic.h>

#include <Arca/Serialization.h>

using namespace Arca;

class RelicParentingTestsFixture : public GeneralFixture
{
public:
    class Shard;
    class GlobalRelic;
};

namespace Arca
{
    template<>
    struct Traits<::RelicParentingTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "RelicTestsShard";
    };

    template<>
    struct Traits<::RelicParentingTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicTestsGlobalRelic";
        static const Locality locality = Locality::Global;
    };
}

class RelicParentingTestsFixture::Shard
{
public:
    std::string myValue;
public:
    Shard() = default;
    explicit Shard(std::string myValue);
};

class RelicParentingTestsFixture::GlobalRelic final : public ClosedTypedRelic<GlobalRelic>
{
public:
    Index<Shard> basicShard;
public:
    explicit GlobalRelic(Init init);
};

namespace Inscription
{
    template<>
    class Scribe<::RelicParentingTestsFixture::Shard, BinaryArchive> final
        : public ArcaCompositeScribe<::RelicParentingTestsFixture::Shard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::RelicParentingTestsFixture::GlobalRelic, BinaryArchive> final
        : public ArcaNullScribe<::RelicParentingTestsFixture::GlobalRelic, BinaryArchive>
    {};
}