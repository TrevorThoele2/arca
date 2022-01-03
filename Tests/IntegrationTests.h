#pragma once

#include "GeneralFixture.h"

#include <Arca/Reliquary.h>

#include "DifferentiableShard.h"
#include "SerializationData.h"

using namespace Arca;

class IntegrationTestsFixture : public GeneralFixture
{
public:
    using BasicShard = DifferentiableShard<0>;
    using OtherShard = DifferentiableShard<1>;

    struct BasicSignal;
    class BasicCuratorBase;

    class MatrixCreatingRelic;

    class RelicListeningToSignalFromConstructor;

    class GlobalRelicCreatingNullSerializedRelic;
};

namespace Arca
{
    template<>
    struct Traits<IntegrationTestsFixture::BasicSignal>
    {
        static constexpr ObjectType objectType = ObjectType::Signal;
        static const inline TypeName typeName = "IntegrationTestsFixture::BasicSignal";
    };

    template<>
    struct Traits<IntegrationTestsFixture::MatrixCreatingRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "IntegrationTestsFixture::MatrixCreatingRelic";
    };

    template<>
    struct Traits<IntegrationTestsFixture::RelicListeningToSignalFromConstructor>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "IntegrationTestsFixture::RelicListeningToSignalFromConstructor";
    };

    template<>
    struct Traits<IntegrationTestsFixture::GlobalRelicCreatingNullSerializedRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "IntegrationTestsFixture::GlobalRelicCreatingNullSerializedRelic";
        static constexpr Locality locality = Locality::Global;
    };
}

struct IntegrationTestsFixture::BasicSignal
{
    int value = 0;
};

class IntegrationTestsFixture::MatrixCreatingRelic final
{
public:
    explicit MatrixCreatingRelic(RelicInit init);
};

class IntegrationTestsFixture::RelicListeningToSignalFromConstructor final
{
public:
    std::vector<int> signalExecutions;
public:
    RelicListeningToSignalFromConstructor(RelicInit init);
};

class IntegrationTestsFixture::GlobalRelicCreatingNullSerializedRelic final
{
public:
    using LocalShard = SerializationData::BasicShardNullInscription;
    using LocalRelic = SerializationData::LocalRelicNullInscription<LocalShard>;
    Index<LocalRelic> localRelic;
public:
    GlobalRelicCreatingNullSerializedRelic(RelicInit init);
    GlobalRelicCreatingNullSerializedRelic(RelicInit init, Serialization);
};

namespace Inscription
{
    template<class Format>
    struct ScribeTraits<IntegrationTestsFixture::MatrixCreatingRelic, Format> final
    {
        using Category = ArcaNullScribeCategory<IntegrationTestsFixture::MatrixCreatingRelic>;
    };

    template<class Format>
    struct ScribeTraits<IntegrationTestsFixture::RelicListeningToSignalFromConstructor, Format> final
    {
        using Category = ArcaNullScribeCategory<IntegrationTestsFixture::RelicListeningToSignalFromConstructor>;
    };

    template<>
    class Scribe<IntegrationTestsFixture::GlobalRelicCreatingNullSerializedRelic> final
    {
    public:
        using ObjectT = IntegrationTestsFixture::GlobalRelicCreatingNullSerializedRelic;
    public:
        template<class Format>
        void Scriven(ObjectT& object, Format& format)
        {
            format("localRelic", object.localRelic);
        }
    };

    template<class Format>
    struct ScribeTraits<IntegrationTestsFixture::GlobalRelicCreatingNullSerializedRelic, Format> final
    {
        using Category = ArcaCompositeScribeCategory<IntegrationTestsFixture::GlobalRelicCreatingNullSerializedRelic>;
    };
}