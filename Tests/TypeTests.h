#pragma once

#include "GeneralFixture.h"

#include <Inscription/BinaryArchive.h>

using namespace Arca;

class TypeTestsFixture : public GeneralFixture
{
public:
    class Shard;
    class TypedRelic;
};

namespace Arca
{
    template<>
    struct Traits<::TypeTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static TypeName TypeName() { return "HandleTestsShard"; }
    };

    template<>
    struct Traits<::TypeTestsFixture::TypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "HandleTestsTypedRelic"; }
    };
}

class TypeTestsFixture::Shard
{
public:
    explicit Shard() = default;
};

class TypeTestsFixture::TypedRelic final
{};

namespace Inscription
{
    template<class Archive>
    struct ScribeTraits<TypeTestsFixture::Shard, Archive> final
    {
        using Category = ArcaNullScribeCategory<TypeTestsFixture::Shard>;
    };

    template<class Archive>
    struct ScribeTraits<TypeTestsFixture::TypedRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<TypeTestsFixture::TypedRelic>;
    };
}