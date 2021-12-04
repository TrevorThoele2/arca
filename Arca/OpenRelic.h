#pragma once

#include "CompositeScribeCategory.h"

namespace Arca
{
    class OpenRelic final
    {
    public:
        OpenRelic() = default;
    };

    template<>
    struct Traits<OpenRelic>
    {
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "Arca::OpenRelic";
    };
}

namespace Inscription
{
    template<>
    class Scribe<Arca::OpenRelic> final
    {
    public:
        using ObjectT = Arca::OpenRelic;
    public:
        template<class Archive>
        void Scriven(ObjectT&, Archive&)
        {}
    };

    template<class Archive>
    struct ScribeTraits<Arca::OpenRelic, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<Arca::OpenRelic>;
    };
}