#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelic.h>

#include <Arca/Serialization.h>

#include "BasicShard.h"

using namespace Arca;

class RelicParentingTestsFixture : public GeneralFixture
{
public:
    class GlobalRelic;
};

namespace Arca
{
    template<>
    struct Traits<::RelicParentingTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicTestsGlobalRelic";
        static const Locality locality = Locality::Global;
    };
}

class RelicParentingTestsFixture::GlobalRelic final : public ClosedTypedRelic<GlobalRelic>
{
public:
    Index<BasicShard> basicShard;
public:
    explicit GlobalRelic(Init init);
};

namespace Inscription
{
    template<class Archive>
    struct ScribeTraits<RelicParentingTestsFixture::GlobalRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<RelicParentingTestsFixture::GlobalRelic>;
    };
}