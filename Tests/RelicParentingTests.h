#pragma once

#include "GeneralFixture.h"

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
        static TypeName TypeName() { return "RelicTestsGlobalRelic"; }
        static const Locality locality = Locality::Global;
    };
}

class RelicParentingTestsFixture::GlobalRelic final
{
public:
    Index<BasicShard> basicShard;
public:
    explicit GlobalRelic(RelicInit init);
};

namespace Inscription
{
    template<class Archive>
    struct ScribeTraits<RelicParentingTestsFixture::GlobalRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<RelicParentingTestsFixture::GlobalRelic>;
    };
}