#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelic.h>

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
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsBasicTypedRelic";
    };
}

class EitherTestsFixture::BasicTypedRelic final : public ClosedTypedRelic<BasicTypedRelic>
{
public:
    Index<BasicShard> basicShard;
public:
    explicit BasicTypedRelic(Init init);
};

namespace Inscription
{
    template<class Archive>
    struct ScribeTraits<EitherTestsFixture::BasicTypedRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<EitherTestsFixture::BasicTypedRelic>;
    };
}