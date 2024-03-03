#pragma once

#include "GeneralFixture.h"

#include <Arca/Shard.h>
#include <Arca/OpenRelic.h>

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
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "IndexTestsTypedClosedRelic";
    };

    template<>
    struct Traits<IndexTestsFixture::TypedOpenRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "IndexTestsTypedOpenRelic";
    };

    template<>
    struct Traits<IndexTestsFixture::GlobalRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "IndexTestsGlobalRelic";
        static constexpr Locality locality = Locality::Global;
    };

    template<>
    struct Traits<IndexTestsFixture::RelicHolderRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "IndexTestsRelicHolderRelic";
    };
}

class IndexTestsFixture::TypedClosedRelic final
{
public:
    Index<BasicShard> shard;
public:
    explicit TypedClosedRelic(RelicInit init);
};

class IndexTestsFixture::TypedOpenRelic final
{
public:
    Index<BasicShard> shard;
public:
    explicit TypedOpenRelic(RelicInit init);
};

class IndexTestsFixture::GlobalRelic final
{
public:
    Index<BasicShard> shard;
public:
    explicit GlobalRelic(RelicInit init);
};

class IndexTestsFixture::RelicHolderRelic final
{
public:
    Index<OpenRelic> held;
public:
    explicit RelicHolderRelic(RelicInit init);
    explicit RelicHolderRelic(RelicInit init, Index<OpenRelic> held);
};

namespace Inscription
{
    template<class Format>
    struct ScribeTraits<IndexTestsFixture::TypedClosedRelic, Format> final
    {
        using Category = ArcaNullScribeCategory<IndexTestsFixture::TypedClosedRelic>;
    };

    template<class Format>
    struct ScribeTraits<IndexTestsFixture::TypedOpenRelic, Format> final
    {
        using Category = ArcaNullScribeCategory<IndexTestsFixture::TypedOpenRelic>;
    };

    template<class Format>
    struct ScribeTraits<IndexTestsFixture::GlobalRelic, Format> final
    {
        using Category = ArcaNullScribeCategory<IndexTestsFixture::GlobalRelic>;
    };

    template<>
    class Scribe<IndexTestsFixture::RelicHolderRelic> final
    {
    public:
        using ObjectT = IndexTestsFixture::RelicHolderRelic;
    public:
        template<class Format>
        void Scriven(ObjectT& object, Format& format)
        {
            format("held", object.held);
        }
    };

    template<class Format>
    struct ScribeTraits<IndexTestsFixture::RelicHolderRelic, Format> final
    {
        using Category = ArcaCompositeScribeCategory<IndexTestsFixture::RelicHolderRelic>;
    };
}