#pragma once

#include "ReliquaryFixture.h"

using namespace Arca;

class IsTypeTestsFixture : public ReliquaryFixture
{
public:
    class Shard;
    class Relic;
    class Curator;
};

class IsTypeTestsFixture::Shard
{};

class IsTypeTestsFixture::Relic
{};

class IsTypeTestsFixture::Curator : public Arca::Curator
{};

namespace Arca
{
    template<>
    struct Traits<::IsTypeTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static TypeName TypeName() { return "IsTypeTestsShard"; }
    };

    template<>
    struct Traits<::IsTypeTestsFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "IsTypeTestsRelic"; }
    };

    template<>
    struct Traits<::IsTypeTestsFixture::Curator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static TypeName TypeName() { return "IsTypeTestsCurator"; }
    };
}

namespace Inscription
{
    template<class Archive>
    struct ScribeTraits<IsTypeTestsFixture::Shard, Archive> final
    {
        using Category = ArcaNullScribeCategory<IsTypeTestsFixture::Shard>;
    };

    template<class Archive>
    struct ScribeTraits<IsTypeTestsFixture::Relic, Archive> final
    {
        using Category = ArcaNullScribeCategory<IsTypeTestsFixture::Relic>;
    };

    template<class Archive>
    struct ScribeTraits<IsTypeTestsFixture::Curator, Archive> final
    {
        using Category = ArcaNullScribeCategory<IsTypeTestsFixture::Curator>;
    };
}