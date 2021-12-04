#pragma once

#include "GeneralFixture.h"

#include "BasicShard.h"

#include <Arca/Serialization.h>

using namespace Arca;

class GlobalRelicTestsFixture : public GeneralFixture
{
public:
    class BasicTypedRelic;
    class GlobalRelic;
};

namespace Arca
{
    template<>
    struct Traits<GlobalRelicTestsFixture::BasicTypedRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "GlobalRelicTestsBasicTypedRelic";
    };

    template<>
    struct Traits<GlobalRelicTestsFixture::GlobalRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "GlobalRelicTestsGlobalRelic";
        static constexpr Locality locality = Locality::Global;
    };
}

class GlobalRelicTestsFixture::BasicTypedRelic final
{
public:
    Index<BasicShard> basicShard;
public:
    explicit BasicTypedRelic(RelicInit init);
};

class GlobalRelicTestsFixture::GlobalRelic final
{
public:
    int myValue = 0;
    Index<BasicShard> basicShard;
public:
    explicit GlobalRelic(RelicInit init);
};

namespace Inscription
{
    template<class Archive>
    struct ScribeTraits<GlobalRelicTestsFixture::BasicTypedRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<GlobalRelicTestsFixture::BasicTypedRelic>;
    };

    template<class Archive>
    struct ScribeTraits<GlobalRelicTestsFixture::GlobalRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<GlobalRelicTestsFixture::GlobalRelic>;
    };
}