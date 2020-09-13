#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelic.h>
#include <Arca/Reliquary.h>

#include <Inscription/BinaryArchive.h>

#include "DifferentiableShard.h"

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
};

namespace Arca
{
    template<>
    struct Traits<::IntegrationTestsFixture::BasicSignal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "IntegrationTestsBasicSignal";
    };

    template<>
    struct Traits<IntegrationTestsFixture::ChildRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "IntegrationTestsChildRelic";
    };

    template<>
    struct Traits<IntegrationTestsFixture::ParentRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "IntegrationTestsParentRelic";
        static bool ShouldCreate(Reliquary& reliquary, int value);
    };

    template<>
    struct Traits<IntegrationTestsFixture::MatrixCreatingRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "IntegrationTestsMatrixCreatingRelic";
    };

    template<>
    struct Traits<IntegrationTestsFixture::MatrixAndParentCurator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "IntegrationTestsParentAndMatrixCurator";
    };
}

struct IntegrationTestsFixture::BasicSignal
{
    int value = 0;
};

class IntegrationTestsFixture::ChildRelic final : public ClosedTypedRelic<ChildRelic>
{
public:
    int value = 0;
public:
    explicit ChildRelic(Init init) : ClosedTypedRelic(init)
    {}
};

class IntegrationTestsFixture::ParentRelic final : public ClosedTypedRelic<ParentRelic>
{
public:
    int value = 0;
public:
    explicit ParentRelic(Init init) : ClosedTypedRelic(init)
    {}

    ParentRelic(Init init, int value);

    void CreateChild() const;
};

class IntegrationTestsFixture::MatrixCreatingRelic final : public ClosedTypedRelic<MatrixCreatingRelic>
{
public:
    explicit MatrixCreatingRelic(Init init);
};

class IntegrationTestsFixture::MatrixAndParentCurator final : public Curator
{
public:
    bool hadMatrixAndParent = false;
public:
    MatrixAndParentCurator(Init init);
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
}