#pragma once

#include "GeneralFixture.h"

#include <Arca/Shard.h>
#include <Arca/ClosedTypedRelic.h>
#include <Arca/OpenTypedRelic.h>

using namespace Arca;

class IndexTestsFixture : public GeneralFixture
{
public:
    struct Shard
    {
        int myInt;
    };

    template<size_t i>
    struct DifferentiableShard
    {
        int myInt;
    };

    class TypedClosedRelic final : public ClosedTypedRelic<TypedClosedRelic>
    {
    public:
        ShardIndex<Shard> shard;
    public:
        explicit TypedClosedRelic(Init init);
    };

    class TypedOpenRelic final : public OpenTypedRelic<TypedOpenRelic>
    {
    public:
        ShardIndex<Shard> shard;
    public:
        explicit TypedOpenRelic(Init init);
    };

    class GlobalRelic final : public ClosedTypedRelic<GlobalRelic>
    {
    public:
        ShardIndex<Shard> shard;
    public:
        explicit GlobalRelic(Init init);
    };
};

namespace Arca
{
    template<>
    struct Traits<IndexTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "IndexTestsShard";
    };

    template<size_t i>
    struct Traits<IndexTestsFixture::DifferentiableShard<i>>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "IndexTestsDifferentiableShard" + Chroma::ToString(i);
    };

    template<>
    struct Traits<IndexTestsFixture::TypedClosedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "IndexTestsTypedClosedRelic";
    };

    template<>
    struct Traits<IndexTestsFixture::TypedOpenRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "IndexTestsTypedOpenRelic";
    };

    template<>
    struct Traits<IndexTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "IndexTestsGlobalRelic";
        static const Locality locality = Locality::Global;
    };
}

namespace Inscription
{
    template<>
    class Scribe<IndexTestsFixture::Shard, BinaryArchive> final :
        public ArcaNullScribe<IndexTestsFixture::Shard, BinaryArchive>
    {};

    template<size_t i>
    class Scribe<IndexTestsFixture::DifferentiableShard<i>, BinaryArchive> final :
        public ArcaNullScribe<IndexTestsFixture::DifferentiableShard<i>, BinaryArchive>
    {};

    template<>
    class Scribe<IndexTestsFixture::TypedClosedRelic, BinaryArchive> final :
        public ArcaNullScribe<IndexTestsFixture::TypedClosedRelic, BinaryArchive>
    {};

    template<>
    class Scribe<IndexTestsFixture::TypedOpenRelic, BinaryArchive> final :
        public ArcaNullScribe<IndexTestsFixture::TypedOpenRelic, BinaryArchive>
    {};

    template<>
    class Scribe<IndexTestsFixture::GlobalRelic, BinaryArchive> final :
        public ArcaNullScribe<IndexTestsFixture::GlobalRelic, BinaryArchive>
    {};
}