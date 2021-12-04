#pragma once

#include "GeneralFixture.h"

#include "BasicShard.h"

#include <Arca/Serialization.h>

using namespace Arca;

class EitherTestsFixture : public GeneralFixture
{
public:
    class BasicTypedRelic;
};

namespace Arca
{
    template<>
    struct Traits<::EitherTestsFixture::BasicTypedRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "ReliquaryTestsBasicTypedRelic";
    };
}

class EitherTestsFixture::BasicTypedRelic final
{
public:
    Index<BasicShard> basicShard;
public:
    explicit BasicTypedRelic(RelicInit init);
};

namespace Inscription
{
    template<class Archive>
    struct ScribeTraits<EitherTestsFixture::BasicTypedRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<EitherTestsFixture::BasicTypedRelic>;
    };
}