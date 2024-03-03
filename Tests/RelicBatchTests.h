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
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicBatchTestsRelic";
    };

    template<>
    struct Traits<::RelicBatchFixture::GlobalRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicBatchTestsGlobalRelic";
    };

    template<>
    struct Traits<::RelicBatchFixture::UnregisteredRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "RelicBatchTestsUnregisteredRelic";
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
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {
            archive("value", object.value);
        }
    };

    template<class Archive>
    struct ScribeTraits<RelicBatchFixture::Relic, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<RelicBatchFixture::Relic>;
    };

    template<>
    class Scribe<RelicBatchFixture::GlobalRelic> final
    {
    public:
        using ObjectT = RelicBatchFixture::GlobalRelic;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {
            archive("value", object.value);
        }
    };

    template<class Archive>
    struct ScribeTraits<RelicBatchFixture::GlobalRelic, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<RelicBatchFixture::GlobalRelic>;
    };
}