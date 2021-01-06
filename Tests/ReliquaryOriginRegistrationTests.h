#pragma once

#include "GeneralFixture.h"

#include <Inscription/BinaryArchive.h>

using namespace Arca;

class ReliquaryOriginRegistrationTestsFixture : public GeneralFixture
{
public:
    class Shard
    {};

    class Relic final
    {
    public:
        Relic() = default;
    };

    class GlobalRelic final
    {
    public:
        GlobalRelic() = default;
    };

    class GlobalRelicWithMovedValue final
    {
    public:
        std::unique_ptr<int> myInt;
    public:
        GlobalRelicWithMovedValue() = default;
        explicit GlobalRelicWithMovedValue(std::unique_ptr<int>&& myInt);
    };

    class Curator final : public Arca::Curator
    {
    public:
        using Arca::Curator::Curator;
    };

    class CuratorWithMovedValue final : public Arca::Curator
    {
    public:
        std::unique_ptr<int> myInt;
    public:
        CuratorWithMovedValue(Init init, std::unique_ptr<int>&& myInt);
    };

    struct Signal
    {};
};

namespace Arca
{
    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::Shard>
    {
        static const ObjectType objectType = ObjectType::Shard;
        static TypeName TypeName() { return "ReliquaryTestsShard"; }
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "ReliquaryTestsRelic"; }
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "ReliquaryTestsGlobalRelic"; }
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::GlobalRelicWithMovedValue>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "ReliquaryTestsGlobalRelicWithMovedValue"; }
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::Curator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static TypeName TypeName() { return "ReliquaryTestsCurator"; }
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::CuratorWithMovedValue>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static TypeName TypeName() { return "ReliquaryTestsCuratorWithMovedValue"; }
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::Signal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static TypeName TypeName() { return "ReliquaryTestsSignal"; }
    };
}

namespace Inscription
{
    template<class Archive>
    struct ScribeTraits<ReliquaryOriginRegistrationTestsFixture::Shard, Archive> final
    {
        using Category = ArcaNullScribeCategory<ReliquaryOriginRegistrationTestsFixture::Shard>;
    };

    template<class Archive>
    struct ScribeTraits<ReliquaryOriginRegistrationTestsFixture::Relic, Archive> final
    {
        using Category = ArcaNullScribeCategory<ReliquaryOriginRegistrationTestsFixture::Relic>;
    };

    template<class Archive>
    struct ScribeTraits<ReliquaryOriginRegistrationTestsFixture::GlobalRelic, Archive> final
    {
        using Category = ArcaNullScribeCategory<ReliquaryOriginRegistrationTestsFixture::GlobalRelic>;
    };

    template<class Archive>
    struct ScribeTraits<ReliquaryOriginRegistrationTestsFixture::GlobalRelicWithMovedValue, Archive> final
    {
        using Category = ArcaNullScribeCategory<ReliquaryOriginRegistrationTestsFixture::GlobalRelicWithMovedValue>;
    };

    template<class Archive>
    struct ScribeTraits<ReliquaryOriginRegistrationTestsFixture::Curator, Archive> final
    {
        using Category = ArcaNullScribeCategory<ReliquaryOriginRegistrationTestsFixture::Curator>;
    };

    template<class Archive>
    struct ScribeTraits<ReliquaryOriginRegistrationTestsFixture::CuratorWithMovedValue, Archive> final
    {
        using Category = ArcaNullScribeCategory<ReliquaryOriginRegistrationTestsFixture::CuratorWithMovedValue>;
    };
}