#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelic.h>

#include <Inscription/BinaryArchive.h>

using namespace Arca;

class ReliquaryTestsFixture : public GeneralFixture
{
public:
    class BasicShard;
    class OtherBasicShard;
    class OtherShard;

    class BasicTypedRelic;
    class GlobalRelic;

    class BasicCurator;

    struct BasicSignal;
};

namespace Arca
{
    template<>
    struct Traits<::ReliquaryTestsFixture::BasicShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ReliquaryTestsBasicShard";
    };

    template<>
    struct Traits<::ReliquaryTestsFixture::OtherBasicShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ReliquaryTestsOtherBasicShard";
    };

    template<>
    struct Traits<::ReliquaryTestsFixture::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "ReliquaryTestsOtherShard";
    };

    template<>
    struct Traits<::ReliquaryTestsFixture::BasicTypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsBasicTypedRelic";
    };

    template<>
    struct Traits<::ReliquaryTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsGlobalRelic";
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::ReliquaryTestsFixture::BasicCurator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "ReliquaryTestsBasicCurator";
    };

    template<>
    struct Traits<::ReliquaryTestsFixture::BasicSignal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "ReliquaryTestsBasicSignal";
    };
}

class ReliquaryTestsFixture::BasicShard
{
public:
    std::string myValue;
public:
    BasicShard() = default;
    explicit BasicShard(std::string myValue);
};

class ReliquaryTestsFixture::OtherBasicShard
{
public:
    std::string myValue;
public:
    OtherBasicShard() = default;
    explicit OtherBasicShard(int myValue);
};

class ReliquaryTestsFixture::OtherShard
{
public:
    int myValue;
public:
    OtherShard() = default;
    explicit OtherShard(int myValue);
};

class ReliquaryTestsFixture::BasicTypedRelic final : public ClosedTypedRelic<BasicTypedRelic>
{
public:
    Index<BasicShard> basicShard;
public:
    explicit BasicTypedRelic(Init init);
};

class ReliquaryTestsFixture::GlobalRelic final : public ClosedTypedRelic<GlobalRelic>
{
public:
    Index<BasicShard> basicShard;
public:
    explicit GlobalRelic(Init init);
};

class ReliquaryTestsFixture::BasicCurator final : public Curator
{
public:
    using Curator::Curator;
};

struct ReliquaryTestsFixture::BasicSignal
{};

namespace Inscription
{
    template<>
    class Scribe<::ReliquaryTestsFixture::BasicShard, BinaryArchive> final
        : public ArcaCompositeScribe<::ReliquaryTestsFixture::BasicShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::OtherBasicShard, BinaryArchive> final
        : public ArcaCompositeScribe<::ReliquaryTestsFixture::OtherBasicShard, BinaryArchive>
    {
    public:
        static std::vector<Type> InputTypes(const ArchiveT& archive)
        {
            return { "ReliquaryTestsBasicShard" };
        }
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::OtherShard, BinaryArchive> final
        : public ArcaCompositeScribe<::ReliquaryTestsFixture::OtherShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::ReliquaryTestsFixture::BasicTypedRelic, BinaryArchive> final
        : public ArcaNullScribe<::ReliquaryTestsFixture::BasicTypedRelic, BinaryArchive>
    {};

    template<>
    class Scribe<::ReliquaryTestsFixture::GlobalRelic, BinaryArchive> final
        : public ArcaNullScribe<::ReliquaryTestsFixture::GlobalRelic, BinaryArchive>
    {};

    template<>
    class Scribe<::ReliquaryTestsFixture::BasicCurator, BinaryArchive> final
        : public ArcaNullScribe<::ReliquaryTestsFixture::BasicCurator, BinaryArchive>
    {};
}