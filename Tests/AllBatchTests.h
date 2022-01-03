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
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "EitherShardBatchTestsRelic";
    };

    template<>
    struct Traits<::AllBatchTestsFixture::GlobalRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "EitherShardBatchTestsGlobalRelic";
        static constexpr Locality locality = Locality::Global;
    };
}

namespace Inscription
{
    template<class Format>
    struct ScribeTraits<AllBatchTestsFixture::Relic, Format> final
    {
        using Category = ArcaNullScribeCategory<AllBatchTestsFixture::Relic>;
    };

    template<class Format>
    struct ScribeTraits<AllBatchTestsFixture::GlobalRelic, Format> final
    {
        using Category = ArcaNullScribeCategory<AllBatchTestsFixture::GlobalRelic>;
    };
}