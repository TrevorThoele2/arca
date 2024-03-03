#pragma once

#include "GeneralFixture.h"

#include <Arca/TypedRelic.h>
#include <Arca/Shard.h>
#include "Inscription/BaseScriven.h"

using namespace Arca;

class RelicTestsFixture : public GeneralFixture
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

    class OtherShard
    {
    public:
        int myValue;
    public:
        OtherShard() = default;
        explicit OtherShard(int myValue);
    };

    class BasicTypedRelic : public TypedRelic<BasicShard>
    {
    public:
        BasicShard* basicShard = nullptr;
    public:
        BasicTypedRelic(RelicID id, Reliquary& owner);
    private:
        void Setup();
    };

    class StaticRelic : public TypedRelic<BasicShard>
    {
    public:
        BasicShard* basicShard = nullptr;
    public:
        StaticRelic(RelicID id, Reliquary& owner);
    private:
        void Setup();
    };
};

namespace Arca
{
    template<>
    struct ShardTraits<::RelicTestsFixture::BasicShard>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct ShardTraits<::RelicTestsFixture::OtherShard>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::RelicTestsFixture::BasicTypedRelic>
    {
        static const TypeHandle typeHandle;
    };

    template<>
    struct RelicTraits<::RelicTestsFixture::StaticRelic>
    {
        static const TypeHandle typeHandle;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::RelicTestsFixture::BasicShard, BinaryArchive> final
        : public ShardScribe<::RelicTestsFixture::BasicShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::RelicTestsFixture::OtherShard, BinaryArchive> final
        : public ShardScribe<::RelicTestsFixture::OtherShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::RelicTestsFixture::BasicTypedRelic, BinaryArchive> final
        : CompositeScribe<::RelicTestsFixture::BasicTypedRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            BaseScriven<TypedRelic<::RelicTestsFixture::BasicShard>>(object, archive);
        }
    };

    template<>
    class Scribe<::RelicTestsFixture::StaticRelic, BinaryArchive> final
        : CompositeScribe<::RelicTestsFixture::StaticRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            BaseScriven<TypedRelic<::RelicTestsFixture::BasicShard>>(object, archive);
        }
    };
}