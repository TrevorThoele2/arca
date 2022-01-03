#pragma once

#include "ReliquaryFixture.h"

using namespace Arca;

class RelicBatchFixture : public ReliquaryFixture
{
public:
    class Relic;
    class GlobalRelic;
    class UnregisteredRelic;
};

class RelicBatchFixture::Relic final
{
public:
    int value = 0;
};

class RelicBatchFixture::GlobalRelic final
{
public:
    int value = 0;
};

class RelicBatchFixture::UnregisteredRelic final
{};

namespace Arca
{
    template<>
    struct Traits<::RelicBatchFixture::Relic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "RelicBatchTestsRelic";
    };

    template<>
    struct Traits<::RelicBatchFixture::GlobalRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "RelicBatchTestsGlobalRelic";
    };

    template<>
    struct Traits<::RelicBatchFixture::UnregisteredRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "RelicBatchTestsUnregisteredRelic";
    };
}

namespace Inscription
{
    template<>
    class Scribe<RelicBatchFixture::Relic> final
    {
    public:
        using ObjectT = RelicBatchFixture::Relic;
    public:
        template<class Format>
        void Scriven(ObjectT& object, Format& format)
        {
            format("value", object.value);
        }
    };

    template<class Format>
    struct ScribeTraits<RelicBatchFixture::Relic, Format> final
    {
        using Category = ArcaCompositeScribeCategory<RelicBatchFixture::Relic>;
    };

    template<>
    class Scribe<RelicBatchFixture::GlobalRelic> final
    {
    public:
        using ObjectT = RelicBatchFixture::GlobalRelic;
    public:
        template<class Format>
        void Scriven(ObjectT& object, Format& format)
        {
            format("value", object.value);
        }
    };

    template<class Format>
    struct ScribeTraits<RelicBatchFixture::GlobalRelic, Format> final
    {
        using Category = ArcaCompositeScribeCategory<RelicBatchFixture::GlobalRelic>;
    };
}