#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelicAutomation.h>
#include <Arca/ShardTraits.h>

#include <Inscription/BinaryArchive.h>

using namespace Arca;

class ReliquaryTestsFixture : public GeneralFixture
{
public:
    class BasicShard
    {
    public:
        std::string myValue;
    public:
        BasicShard() = default;
        explicit BasicShard(std::string myValue);
    };

    class OtherBasicShard
    {
    public:
        std::string myValue;
    public:
        OtherBasicShard() = default;
        explicit OtherBasicShard(int myValue);
    };

    class OtherShard
    {
    public:
        int myValue;
    public:
        OtherShard() = default;
        explicit OtherShard(int myValue);
    };

    class BasicTypedRelic : public ClosedTypedRelicAutomation<BasicTypedRelic, BasicShard>
    {
    public:
        Ptr<BasicShard> basicShard;
    public:
        BasicTypedRelic() = default;
    protected:
        void InitializeImplementation() override;
    };

    class GlobalRelic : public ClosedTypedRelicAutomation<GlobalRelic, BasicShard>
    {
    public:
        Ptr<BasicShard> basicShard;
    public:
        GlobalRelic() = default;
    protected:
        void InitializeImplementation() override;
    };

    class BasicCurator final : public Curator
    {};

    struct BasicSignal
    {};
};

namespace Arca
{
    template<>
    struct Traits<::ReliquaryTestsFixture::BasicShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ReliquaryTestsFixture::OtherBasicShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ReliquaryTestsFixture::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ReliquaryTestsFixture::BasicTypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ReliquaryTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::ReliquaryTestsFixture::BasicCurator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::ReliquaryTestsFixture::BasicSignal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static const TypeName typeName;
    };
}

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
    class Scribe<::ReliquaryTestsFixture::BasicCurator, BinaryArchive> final :
        public ArcaNullScribe<::ReliquaryTestsFixture::BasicCurator, BinaryArchive>
    {};
}