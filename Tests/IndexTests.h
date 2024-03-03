#pragma once

#include "GeneralFixture.h"

#include <Arca/Shard.h>
#include <Arca/ClosedTypedRelic.h>
#include <Arca/OpenTypedRelic.h>

#include "BasicShard.h"

using namespace Arca;

class IndexTestsFixture : public GeneralFixture
{
public:
    class TypedClosedRelic;
    class TypedOpenRelic;
    class GlobalRelic;

    class RelicHolderRelic;
};

namespace Arca
{
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

class IndexTestsFixture::TypedClosedRelic final : public ClosedTypedRelic<TypedClosedRelic>
{
public:
    Index<BasicShard> shard;
public:
    explicit TypedClosedRelic(Init init);
};

class IndexTestsFixture::TypedOpenRelic final : public OpenTypedRelic<TypedOpenRelic>
{
public:
    Index<BasicShard> shard;
public:
    explicit TypedOpenRelic(Init init);
};

class IndexTestsFixture::GlobalRelic final : public ClosedTypedRelic<GlobalRelic>
{
public:
    Index<BasicShard> shard;
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
    template<class Archive>
    struct ScribeTraits<IndexTestsFixture::TypedClosedRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<IndexTestsFixture::TypedClosedRelic>;
    };

    template<class Archive>
    struct ScribeTraits<IndexTestsFixture::TypedOpenRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<IndexTestsFixture::TypedOpenRelic>;
    };

    template<class Archive>
    struct ScribeTraits<IndexTestsFixture::GlobalRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<IndexTestsFixture::GlobalRelic>;
    };

    template<>
    class Scribe<IndexTestsFixture::RelicHolderRelic> final
    {
    public:
        using ObjectT = IndexTestsFixture::RelicHolderRelic;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {
            archive("held", object.held);
        }
    };

    template<class Archive>
    struct ScribeTraits<IndexTestsFixture::RelicHolderRelic, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<IndexTestsFixture::RelicHolderRelic>;
    };
}