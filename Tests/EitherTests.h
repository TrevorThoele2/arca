#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelic.h>

#include <Arca/Serialization.h>

using namespace Arca;

class EitherTestsFixture : public GeneralFixture
{
public:
    class BasicShard;

    class BasicTypedRelic;
};

namespace Arca
{
    template<>
    struct Traits<::EitherTestsFixture::BasicShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "RelicTestsBasicShard";
    };

    template<>
    struct Traits<::EitherTestsFixture::BasicTypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsBasicTypedRelic";
    };
}

class EitherTestsFixture::BasicShard
{
public:
    std::string myValue;
public:
    explicit BasicShard() = default;
    explicit BasicShard(std::string myValue);
};

class EitherTestsFixture::BasicTypedRelic final : public ClosedTypedRelic<BasicTypedRelic>
{
public:
    Index<BasicShard> basicShard;
public:
    explicit BasicTypedRelic(Init init);
};

namespace Inscription
{
    template<>
    class Scribe<::EitherTestsFixture::BasicShard, BinaryArchive> final
        : public ArcaCompositeScribe<::EitherTestsFixture::BasicShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::EitherTestsFixture::BasicTypedRelic, BinaryArchive> final
        : public ArcaNullScribe<::EitherTestsFixture::BasicTypedRelic, BinaryArchive>
    {};
}