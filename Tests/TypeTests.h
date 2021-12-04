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
        static constexpr ObjectType objectType = ObjectType::Shard;
        static const inline TypeName typeName = "HandleTestsShard";
    };

    template<>
    struct Traits<::TypeTestsFixture::TypedRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "HandleTestsTypedRelic";
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