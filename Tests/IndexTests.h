#pragma once

#include "GeneralFixture.h"

#include <Arca/Shard.h>
#include <Arca/ClosedTypedRelic.h>
#include <Arca/OpenTypedRelic.h>

using namespace Arca;

class IndexTestsFixture : public GeneralFixture
{
public:
    struct Shard;
    template<size_t i>
    struct DifferentiableShard;

    class TypedClosedRelic;
    class TypedOpenRelic;
    class GlobalRelic;

    class RelicHolderRelic;
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

    template<>
    struct Traits<IndexTestsFixture::RelicHolderRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "IndexTestsRelicHolderRelic";
    };
}

struct IndexTestsFixture::Shard
{
    int myInt = 0;
    Shard() = default;
    Shard(int myInt);
};

template<size_t i>
struct IndexTestsFixture::DifferentiableShard
{
    int myInt = 0;
};

class IndexTestsFixture::TypedClosedRelic final : public ClosedTypedRelic<TypedClosedRelic>
{
public:
    Index<Shard> shard;
public:
    explicit TypedClosedRelic(Init init);
};

class IndexTestsFixture::TypedOpenRelic final : public OpenTypedRelic<TypedOpenRelic>
{
public:
    Index<Shard> shard;
public:
    explicit TypedOpenRelic(Init init);
};

class IndexTestsFixture::GlobalRelic final : public ClosedTypedRelic<GlobalRelic>
{
public:
    Index<Shard> shard;
public:
    explicit GlobalRelic(Init init);
};

class IndexTestsFixture::RelicHolderRelic final : public ClosedTypedRelic<RelicHolderRelic>
{
public:
    Index<OpenRelic> held;
public:
    explicit RelicHolderRelic(Init init);
    explicit RelicHolderRelic(Init init, Index<OpenRelic> held);
};

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

    template<>
    class Scribe<IndexTestsFixture::RelicHolderRelic, BinaryArchive> final :
        public ArcaCompositeScribe<IndexTestsFixture::RelicHolderRelic, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };
}