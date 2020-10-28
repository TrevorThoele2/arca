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
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsBasicTypedRelic";
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
    template<class Archive>
    struct ScribeTraits<AllTestsFixture::BasicTypedRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<AllTestsFixture::BasicTypedRelic>;
    };
}