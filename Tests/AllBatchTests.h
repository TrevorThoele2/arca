#pragma once

#include "ReliquaryFixture.h"

using namespace Arca;

class AllBatchTestsFixture : public ReliquaryFixture
{
public:
    class Relic;
    class GlobalRelic;
};

class AllBatchTestsFixture::Relic final
{
public:
    explicit Relic(RelicInit init);
};

class AllBatchTestsFixture::GlobalRelic final
{
public:
    explicit GlobalRelic(RelicInit init);
};

namespace Arca
{
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
    template<class Archive>
    struct ScribeTraits<AllBatchTestsFixture::Relic, Archive> final
    {
        using Category = ArcaNullScribeCategory<AllBatchTestsFixture::Relic>;
    };

    template<class Archive>
    struct ScribeTraits<AllBatchTestsFixture::GlobalRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<AllBatchTestsFixture::GlobalRelic>;
    };
}