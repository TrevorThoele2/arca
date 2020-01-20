#pragma once

#include "ReliquaryFixture.h"

#include <Arca/ClosedTypedRelicAutomation.h>

using namespace Arca;

class AllBatchTestsFixture : public ReliquaryFixture
{
public:
    template<size_t i>
    class Shard;
    class Relic;
    class GlobalRelic;
};

template<size_t i>
class AllBatchTestsFixture::Shard
{
public:
    int value = 0;
public:
    Shard() = default;
    explicit Shard(int value);
};

template<size_t i>
AllBatchTestsFixture::Shard<i>::Shard(int value) :
    value(value)
{}

class AllBatchTestsFixture::Relic final :
    public ClosedTypedRelicAutomation<Relic>
{
public:
    void Initialize();
};

class AllBatchTestsFixture::GlobalRelic final :
    public ClosedTypedRelicAutomation<GlobalRelic>
{
public:
    void Initialize();
};

namespace Arca
{
    template<size_t i>
    struct Traits<::AllBatchTestsFixture::Shard<i>>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "EitherShardBatchTestsShard" + ::Chroma::ToString(i);
    };

    template<>
    struct Traits<::AllBatchTestsFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "EitherShardBatchTestsRelic";
    };

    template<>
    struct Traits<::AllBatchTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "EitherShardBatchTestsGlobalRelic";
        static const Locality locality = Locality::Global;
    };
}

namespace Inscription
{
    template<size_t i>
    class Scribe<::AllBatchTestsFixture::Shard<i>, BinaryArchive> final
        : public ArcaCompositeScribe<::AllBatchTestsFixture::Shard<i>, BinaryArchive>
    {
    private:
        using BaseT = ArcaCompositeScribe<::AllBatchTestsFixture::Shard<i>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;

        using BaseT::Scriven;
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.value);
        }
    };

    template<>
    class Scribe<::AllBatchTestsFixture::Relic, BinaryArchive> final
        : public ArcaNullScribe<::AllBatchTestsFixture::Relic, BinaryArchive>
    {};

    template<>
    class Scribe<::AllBatchTestsFixture::GlobalRelic, BinaryArchive> final
        : public ArcaNullScribe<::AllBatchTestsFixture::GlobalRelic, BinaryArchive>
    {};
}