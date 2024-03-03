#pragma once

#include "ReliquaryFixture.h"

#include <Arca/ClosedTypedRelic.h>

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

class IsTypeTestsFixture::Relic : public ClosedTypedRelic
{};

class IsTypeTestsFixture::Curator : public Arca::Curator
{};

namespace Arca
{
    template<>
    struct Traits<::IsTypeTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "IsTypeTestsShard";
    };

    template<>
    struct Traits<::IsTypeTestsFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "IsTypeTestsRelic";
    };

    template<>
    struct Traits<::IsTypeTestsFixture::Curator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "IsTypeTestsCurator";
    };
}

namespace Inscription
{
    template<>
    class Scribe<::IsTypeTestsFixture::Shard, BinaryArchive> final
        : public ArcaNullScribe<::IsTypeTestsFixture::Shard, BinaryArchive>
    {};

    template<>
    class Scribe<::IsTypeTestsFixture::Relic, BinaryArchive> final
        : public ArcaNullScribe<::IsTypeTestsFixture::Relic, BinaryArchive>
    {};
}