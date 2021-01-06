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
    class ChildRelic;
    class ParentRelic;
    class BasicCuratorBase;

    class MatrixCreatingRelic;
    class MatrixAndParentCurator;

    class RelicListeningToSignalFromConstructor;

    class GlobalRelicCreatingNullSerializedRelic;
};

namespace Arca
{
    template<>
    struct Traits<IntegrationTestsFixture::BasicSignal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static TypeName TypeName() { return "IntegrationTestsFixture::BasicSignal"; }
    };

    template<>
    struct Traits<IntegrationTestsFixture::ChildRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "IntegrationTestsFixture::ChildRelic"; }
    };

    template<>
    struct Traits<IntegrationTestsFixture::ParentRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "IntegrationTestsFixture::ParentRelic"; }
        static bool ShouldCreate(Reliquary& reliquary, int value);
    };

    template<>
    struct Traits<IntegrationTestsFixture::MatrixCreatingRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "IntegrationTestsFixture::MatrixCreatingRelic"; }
    };

    template<>
    struct Traits<IntegrationTestsFixture::MatrixAndParentCurator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static TypeName TypeName() { return "IntegrationTestsFixture::ParentAndMatrixCurator"; }
    };

    template<>
    struct Traits<IntegrationTestsFixture::RelicListeningToSignalFromConstructor>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "IntegrationTestsFixture::RelicListeningToSignalFromConstructor"; }
    };

    template<>
    struct Traits<IntegrationTestsFixture::GlobalRelicCreatingNullSerializedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "IntegrationTestsFixture::GlobalRelicCreatingNullSerializedRelic"; }
        static const Locality locality = Locality::Global;
    };
}

struct IntegrationTestsFixture::BasicSignal
{
    int value = 0;
};

class IntegrationTestsFixture::ChildRelic final
{
public:
    int value = 0;
public:
    ChildRelic() = default;
};

class IntegrationTestsFixture::ParentRelic final
{
public:
    int value = 0;
public:
    explicit ParentRelic() = default;

    ParentRelic(int value);
};

class IntegrationTestsFixture::MatrixCreatingRelic final
{
public:
    explicit MatrixCreatingRelic(RelicInit init);
};

class IntegrationTestsFixture::MatrixAndParentCurator final : public Curator
{
public:
    bool hadMatrixAndParent = false;
public:
    MatrixAndParentCurator(Init init);
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
    using LocalRelic = SerializationData::TypedClosedRelicNullInscription<LocalShard>;
    Index<LocalRelic> localRelic;
public:
    GlobalRelicCreatingNullSerializedRelic(RelicInit init);
    GlobalRelicCreatingNullSerializedRelic(RelicInit init, Serialization);
};

namespace Inscription
{
    template<class Archive>
    struct ScribeTraits<IntegrationTestsFixture::ChildRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<IntegrationTestsFixture::ChildRelic>;
    };

    template<class Archive>
    struct ScribeTraits<IntegrationTestsFixture::ParentRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<IntegrationTestsFixture::ParentRelic>;
    };

    template<class Archive>
    struct ScribeTraits<IntegrationTestsFixture::MatrixCreatingRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<IntegrationTestsFixture::MatrixCreatingRelic>;
    };

    template<class Archive>
    struct ScribeTraits<IntegrationTestsFixture::MatrixAndParentCurator, Archive> final
    {
        using Category = ArcaNullScribeCategory<IntegrationTestsFixture::MatrixAndParentCurator>;
    };

    template<class Archive>
    struct ScribeTraits<IntegrationTestsFixture::RelicListeningToSignalFromConstructor, Archive> final
    {
        using Category = ArcaNullScribeCategory<IntegrationTestsFixture::RelicListeningToSignalFromConstructor>;
    };

    template<>
    class Scribe<IntegrationTestsFixture::GlobalRelicCreatingNullSerializedRelic> final
    {
    public:
        using ObjectT = IntegrationTestsFixture::GlobalRelicCreatingNullSerializedRelic;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {
            archive("localRelic", object.localRelic);
        }
    };

    template<class Archive>
    struct ScribeTraits<IntegrationTestsFixture::GlobalRelicCreatingNullSerializedRelic, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<IntegrationTestsFixture::GlobalRelicCreatingNullSerializedRelic>;
    };
}