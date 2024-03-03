#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelicAutomation.h>

#include <Inscription/BinaryArchive.h>

#include <Chroma/StringUtility.h>

using namespace Arca;

class IntegrationTestsFixture : public GeneralFixture
{
public:
    class BasicShard;
    class OtherShard;
    struct BasicSignal;
    class ChildRelic;
    class ParentRelic;
    class BasicCuratorBase;
    template<size_t differentiator>
    class BasicCurator;
};

namespace Arca
{
    template<>
    struct Traits<::IntegrationTestsFixture::BasicShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "IntegrationTestsBasicShard";
    };

    template<>
    struct Traits<::IntegrationTestsFixture::OtherShard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static inline const TypeName typeName = "IntegrationTestsOtherShard";
    };

    template<>
    struct Traits<::IntegrationTestsFixture::BasicSignal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "IntegrationTestsBasicSignal";
    };

    template<>
    struct Traits<::IntegrationTestsFixture::ChildRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "IntegrationTestsChildRelic";
    };

    template<>
    struct Traits<::IntegrationTestsFixture::ParentRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "IntegrationTestsParentRelic";
        static bool ShouldCreate(Reliquary& reliquary, int value);
    };

    template<size_t differentiator>
    struct Traits<::IntegrationTestsFixture::BasicCurator<differentiator>>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName =
            "IntegrationTestsBasicCurator" + ::Chroma::ToString(differentiator);
    };
}

class IntegrationTestsFixture::BasicShard
{
public:
    std::string myValue;
public:
    BasicShard() = default;
    explicit BasicShard(std::string myValue);
};

class IntegrationTestsFixture::OtherShard
{
public:
    int myValue;
public:
    OtherShard() = default;
    explicit OtherShard(int myValue);
};

struct IntegrationTestsFixture::BasicSignal
{
    int value = 0;
};

class IntegrationTestsFixture::ChildRelic : public ClosedTypedRelicAutomation<ChildRelic>
{
public:
    int value = 0;
public:
    ChildRelic() = default;
};

class IntegrationTestsFixture::ParentRelic : public ClosedTypedRelicAutomation<ParentRelic>
{
public:
    int value;
public:
    ParentRelic() = default;

    void Initialize(int value);

    void CreateChild();
};

class IntegrationTestsFixture::BasicCuratorBase : public Curator
{
public:
    bool shouldAbort = false;
    static std::function<void(BasicCuratorBase&)> onPostConstruct;
    static std::function<void(BasicCuratorBase&)> onInitialize;
    std::function<void(BasicCuratorBase&)> onWork;

    Batch<BasicSignal> basicSignals;

    Reliquary& Owner();
public:
    BasicCuratorBase();
protected:
    void PostConstructImplementation() override;
    void InitializeImplementation() override;
    void WorkImplementation(Stage& stage) override;
};

template<size_t differentiator>
class IntegrationTestsFixture::BasicCurator final : public BasicCuratorBase
{
public:
    BasicCurator() = default;
};

namespace Inscription
{
    template<>
    class Scribe<::IntegrationTestsFixture::BasicShard, BinaryArchive> final
        : public ArcaCompositeScribe<::IntegrationTestsFixture::BasicShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::IntegrationTestsFixture::OtherShard, BinaryArchive> final
        : public ArcaCompositeScribe<::IntegrationTestsFixture::OtherShard, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.myValue);
        }
    };

    template<>
    class Scribe<::IntegrationTestsFixture::ChildRelic, BinaryArchive> final
        : public ArcaNullScribe<::IntegrationTestsFixture::ChildRelic, BinaryArchive>
    {};

    template<>
    class Scribe<::IntegrationTestsFixture::ParentRelic, BinaryArchive> final
        : public ArcaNullScribe<::IntegrationTestsFixture::ParentRelic, BinaryArchive>
    {};

    template<size_t differentiator>
    class Scribe<::IntegrationTestsFixture::BasicCurator<differentiator>, BinaryArchive> final :
        public ArcaNullScribe<::IntegrationTestsFixture::BasicCurator<differentiator>, BinaryArchive>
    {
    private:
        using BaseT = ArcaNullScribe<::IntegrationTestsFixture::BasicCurator<differentiator>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;

        using BaseT::Scriven;
    };
}