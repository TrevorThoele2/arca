#pragma once

#include "GeneralFixture.h"

#include "DifferentiableShard.h"

#include <Arca/Serialization.h>

using namespace Arca;

class AllTestsFixture : public GeneralFixture
{
public:
    class BasicTypedRelic;
};

namespace Arca
{
    template<>
    struct Traits<AllTestsFixture::BasicTypedRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "ReliquaryTestsBasicTypedRelic";
    };
}

class AllTestsFixture::BasicTypedRelic final
{
public:
    Index<DifferentiableShard<0>> shard0;
    Index<DifferentiableShard<1>> shard1;
    Index<DifferentiableShard<2>> shard2;
public:
    explicit BasicTypedRelic(RelicInit init);
};

namespace Inscription
{
    template<class Format>
    struct ScribeTraits<AllTestsFixture::BasicTypedRelic, Format> final
    {
        using Category = ArcaNullScribeCategory<AllTestsFixture::BasicTypedRelic>;
    };
}