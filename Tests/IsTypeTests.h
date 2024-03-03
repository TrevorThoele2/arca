#pragma once

#include "ReliquaryFixture.h"

#include <Arca/ShardTraits.h>
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
        static const TypeName typeName;
    };

    template<>
    struct Traits<::IsTypeTestsFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static const TypeName typeName;
    };

    template<>
    struct Traits<::IsTypeTestsFixture::Curator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static const TypeName typeName;
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

    template<>
    class Scribe<::IsTypeTestsFixture::Curator, BinaryArchive> final
        : public ArcaNullScribe<::IsTypeTestsFixture::Curator, BinaryArchive>
    {};
}