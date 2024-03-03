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
        static constexpr ObjectType objectType = ObjectType::Shard;
        static const inline TypeName typeName = "IsTypeTestsShard";
    };

    template<>
    struct Traits<::IsTypeTestsFixture::Relic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "IsTypeTestsRelic";
    };

    template<>
    struct Traits<::IsTypeTestsFixture::Curator>
    {
        static constexpr ObjectType objectType = ObjectType::Curator;
        static const inline TypeName typeName = "IsTypeTestsCurator";
    };
}

namespace Inscription
{
    template<class Format>
    struct ScribeTraits<IsTypeTestsFixture::Shard, Format> final
    {
        using Category = ArcaNullScribeCategory<IsTypeTestsFixture::Shard>;
    };

    template<class Format>
    struct ScribeTraits<IsTypeTestsFixture::Relic, Format> final
    {
        using Category = ArcaNullScribeCategory<IsTypeTestsFixture::Relic>;
    };

    template<class Format>
    struct ScribeTraits<IsTypeTestsFixture::Curator, Format> final
    {
        using Category = ArcaNullScribeCategory<IsTypeTestsFixture::Curator>;
    };
}