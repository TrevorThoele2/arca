#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelic.h>

#include <Inscription/BinaryArchive.h>

using namespace Arca;

class IntegrationTestsFixture : public GeneralFixture
{
public:
    class BasicShard;
    class OtherShard;
    struct BasicSignal;
    class ChildRelic;
    class ParentRelic;
    class BasicCuratorBase;
};

namespace Arca
{
    template<>
    struct Traits<::IntegrationTestsFixture::BasicShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "IntegrationTestsBasicShard";
    };

    template<>
    struct Traits<::IntegrationTestsFixture::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "IntegrationTestsOtherShard";
    };

    template<>
    struct Traits<::IntegrationTestsFixture::BasicSignal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "IntegrationTestsBasicSignal";
    };

    template<>
    struct Traits<::IntegrationTestsFixture::ChildRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "IntegrationTestsChildRelic";
    };

    template<>
    struct Traits<::IntegrationTestsFixture::ParentRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "IntegrationTestsParentRelic";
        static bool ShouldCreate(Reliquary& reliquary, int value);
    };
}

class IntegrationTestsFixture::BasicShard
{
public:
    std::string myValue;
public:
    BasicShard() = default;
    explicit BasicShard(std::string myValue);
};

class IntegrationTestsFixture::OtherShard
{
public:
    int myValue;
public:
    OtherShard() = default;
    explicit OtherShard(int myValue);
};

struct IntegrationTestsFixture::BasicSignal
{
    int value = 0;
};

class IntegrationTestsFixture::ChildRelic final : public ClosedTypedRelic<ChildRelic>
{
public:
    int value = 0;
public:
    explicit ChildRelic(Init init) : ClosedTypedRelic(init)
    {}
};

class IntegrationTestsFixture::ParentRelic final : public ClosedTypedRelic<ParentRelic>
{
public:
    int value = 0;
public:
    explicit ParentRelic(Init init) : ClosedTypedRelic(init)
    {}

    ParentRelic(Init init, int value);

    void CreateChild() const;
};

namespace Inscription
{
    template<>
    class Scribe<::IntegrationTestsFixture::BasicShard, BinaryArchive> final
        : public ArcaCompositeScribe<::IntegrationTestsFixture::BasicShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::IntegrationTestsFixture::OtherShard, BinaryArchive> final
        : public ArcaCompositeScribe<::IntegrationTestsFixture::OtherShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::IntegrationTestsFixture::ChildRelic, BinaryArchive> final
        : public ArcaNullScribe<::IntegrationTestsFixture::ChildRelic, BinaryArchive>
    {};

    template<>
    class Scribe<::IntegrationTestsFixture::ParentRelic, BinaryArchive> final
        : public ArcaNullScribe<::IntegrationTestsFixture::ParentRelic, BinaryArchive>
    {};
}