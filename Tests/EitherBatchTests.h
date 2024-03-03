#pragma once

#include "ReliquaryFixture.h"

#include <Arca/ClosedTypedRelic.h>

using namespace Arca;

class EitherBatchTestsFixture : public ReliquaryFixture
{
public:
    class Relic;
    class GlobalRelic;
};

class EitherBatchTestsFixture::Relic final : public ClosedTypedRelic<Relic>
{
public:
    explicit Relic(Init init);
};

class EitherBatchTestsFixture::GlobalRelic final : public ClosedTypedRelic<GlobalRelic>
{
public:
    explicit GlobalRelic(Init init);
};

namespace Arca
{
    template<>
    struct Traits<::EitherBatchTestsFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "EitherShardBatchTestsRelic";
    };

    template<>
    struct Traits<::EitherBatchTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "EitherShardBatchTestsGlobalRelic";
        static const Locality locality = Locality::Global;
    };
}

namespace Inscription
{
    template<class Archive>
    struct ScribeTraits<EitherBatchTestsFixture::Relic, Archive> final
    {
        using Category = ArcaNullScribeCategory<EitherBatchTestsFixture::Relic>;
    };

    template<class Archive>
    struct ScribeTraits<EitherBatchTestsFixture::GlobalRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<EitherBatchTestsFixture::GlobalRelic>;
    };
}