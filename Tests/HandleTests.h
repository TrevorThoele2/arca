#pragma once

#include "GeneralFixture.h"

#include "DifferentiableShard.h"

#include <Inscription/Binary.h>

using namespace Arca;

class HandleTestsFixture : public GeneralFixture
{
public:
    using Shard = DifferentiableShard<0>;
    using OtherShard = DifferentiableShard<1>;

    class TypedRelic;
    class GlobalRelic;
    class HandleHolder;

    class Curator;

    struct Signal;
};

namespace Arca
{
    template<>
    struct Traits<HandleTestsFixture::Shard>
    {
        static constexpr ObjectType objectType = ObjectType::Shard;
        static const inline TypeName typeName = "HandleTestsShard";
    };

    template<>
    struct Traits<HandleTestsFixture::OtherShard>
    {
        static constexpr ObjectType objectType = ObjectType::Shard;
        static const inline TypeName typeName = "HandleTestsOtherShard";
    };

    template<>
    struct Traits<HandleTestsFixture::TypedRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "HandleTestsTypedRelic";
    };

    template<>
    struct Traits<HandleTestsFixture::GlobalRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "HandleTestsGlobalRelic";
        static constexpr Locality locality = Locality::Global;
    };

    template<>
    struct Traits<HandleTestsFixture::HandleHolder>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "HandleTestsHandleHolder";
    };

    template<>
    struct Traits<HandleTestsFixture::Curator>
    {
        static constexpr ObjectType objectType = ObjectType::Curator;
        static const inline TypeName typeName = "HandleTestsBasicCurator";
    };

    template<>
    struct Traits<HandleTestsFixture::Signal>
    {
        static constexpr ObjectType objectType = ObjectType::Signal;
        static const inline TypeName typeName = "HandleTestsBasicSignal";
    };
}

class HandleTestsFixture::TypedRelic final
{
public:
    Index<Shard> basicShard;
public:
    explicit TypedRelic(RelicInit init);
    TypedRelic(RelicInit init, Serialization);
};

class HandleTestsFixture::GlobalRelic final
{
public:
    Index<Shard> basicShard;
public:
    explicit GlobalRelic(RelicInit init);
};

class HandleTestsFixture::HandleHolder final
{
public:
    Handle handle;
public:
    explicit HandleHolder(RelicInit init);
    explicit HandleHolder(RelicInit init, Handle handle);
};

class HandleTestsFixture::Curator final : public Arca::Curator
{
public:
    using Arca::Curator::Curator;
};

struct HandleTestsFixture::Signal
{};

namespace Inscription
{
    template<class Format>
    struct ScribeTraits<HandleTestsFixture::TypedRelic, Format> final
    {
        using Category = ArcaNullScribeCategory<HandleTestsFixture::TypedRelic>;
    };

    template<class Format>
    struct ScribeTraits<HandleTestsFixture::GlobalRelic, Format> final
    {
        using Category = ArcaNullScribeCategory<HandleTestsFixture::GlobalRelic>;
    };

    template<>
    class Scribe<HandleTestsFixture::HandleHolder> final
    {
    public:
        using ObjectT = HandleTestsFixture::HandleHolder;
    public:
        template<class Format>
        void Scriven(ObjectT& object, Format& format)
        {
            format("handle", object.handle);
        }
    };

    template<class Format>
    struct ScribeTraits<HandleTestsFixture::HandleHolder, Format> final
    {
        using Category = ArcaCompositeScribeCategory<HandleTestsFixture::HandleHolder>;
    };

    template<class Format>
    struct ScribeTraits<HandleTestsFixture::Curator, Format> final
    {
        using Category = ArcaNullScribeCategory<HandleTestsFixture::Curator>;
    };
}