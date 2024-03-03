#pragma once

#include "GeneralFixture.h"

#include "DifferentiableShard.h"

#include <Inscription/BinaryArchive.h>

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
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "HandleTestsShard";
    };

    template<>
    struct Traits<HandleTestsFixture::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "HandleTestsOtherShard";
    };

    template<>
    struct Traits<HandleTestsFixture::TypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "HandleTestsTypedRelic";
    };

    template<>
    struct Traits<HandleTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "HandleTestsGlobalRelic";
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<HandleTestsFixture::HandleHolder>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "HandleTestsHandleHolder";
    };

    template<>
    struct Traits<HandleTestsFixture::Curator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "HandleTestsBasicCurator";
    };

    template<>
    struct Traits<HandleTestsFixture::Signal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "HandleTestsBasicSignal";
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
    template<class Archive>
    struct ScribeTraits<HandleTestsFixture::TypedRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<HandleTestsFixture::TypedRelic>;
    };

    template<class Archive>
    struct ScribeTraits<HandleTestsFixture::GlobalRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<HandleTestsFixture::GlobalRelic>;
    };

    template<>
    class Scribe<HandleTestsFixture::HandleHolder> final
    {
    public:
        using ObjectT = HandleTestsFixture::HandleHolder;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {
            archive("handle", object.handle);
        }
    };

    template<class Archive>
    struct ScribeTraits<HandleTestsFixture::HandleHolder, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<HandleTestsFixture::HandleHolder>;
    };

    template<class Archive>
    struct ScribeTraits<HandleTestsFixture::Curator, Archive> final
    {
        using Category = ArcaNullScribeCategory<HandleTestsFixture::Curator>;
    };
}