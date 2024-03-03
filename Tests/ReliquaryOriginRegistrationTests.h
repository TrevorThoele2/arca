#pragma once

#include "GeneralFixture.h"

#include <Arca/ClosedTypedRelic.h>

#include <Inscription/BinaryArchive.h>

using namespace Arca;

class ReliquaryOriginRegistrationTestsFixture : public GeneralFixture
{
public:
    class Shard
    {};

    class Relic final : public ClosedTypedRelic<Relic>
    {
    public:
        explicit Relic(Init init) : ClosedTypedRelic(init)
        {}
    };

    class GlobalRelic final : public ClosedTypedRelic<GlobalRelic>
    {
    public:
        explicit GlobalRelic(Init init) : ClosedTypedRelic(init)
        {}
    };

    class GlobalRelicWithMovedValue final : public ClosedTypedRelic<GlobalRelicWithMovedValue>
    {
    public:
        std::unique_ptr<int> myInt;
    public:
        explicit GlobalRelicWithMovedValue(Init init) : ClosedTypedRelic(init)
        {}

        GlobalRelicWithMovedValue(Init init, std::unique_ptr<int>&& myInt) :
            ClosedTypedRelic(init), myInt(std::move(myInt))
        {}
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
        static inline const TypeName typeName = "ReliquaryTestsShard";
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::Relic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsRelic";
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsGlobalRelic";
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::GlobalRelicWithMovedValue>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "ReliquaryTestsGlobalRelicWithMovedValue";
        static const Locality locality = Locality::Global;
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::Curator>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "ReliquaryTestsCurator";
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::CuratorWithMovedValue>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "ReliquaryTestsCuratorWithMovedValue";
    };

    template<>
    struct Traits<::ReliquaryOriginRegistrationTestsFixture::Signal>
    {
        static const ObjectType objectType = ObjectType::Signal;
        static inline const TypeName typeName = "ReliquaryTestsSignal";
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