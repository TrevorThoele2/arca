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
        static constexpr ObjectType objectType = ObjectType::Shard;
        static const inline TypeName typeName = "ReliquaryTestsShard";
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::Relic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "ReliquaryTestsRelic";
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::GlobalRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "ReliquaryTestsGlobalRelic";
        static constexpr Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::GlobalRelicWithMovedValue>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "ReliquaryTestsGlobalRelicWithMovedValue";
        static constexpr Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::Curator>
    {
        static constexpr ObjectType objectType = ObjectType::Curator;
        static const inline TypeName typeName = "ReliquaryTestsCurator";
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::CuratorWithMovedValue>
    {
        static constexpr ObjectType objectType = ObjectType::Curator;
        static const inline TypeName typeName = "ReliquaryTestsCuratorWithMovedValue";
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::Signal>
    {
        static constexpr ObjectType objectType = ObjectType::Signal;
        static const inline TypeName typeName = "ReliquaryTestsSignal";
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