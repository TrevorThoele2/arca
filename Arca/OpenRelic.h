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
        template<class Format>
        void Scriven(ObjectT&, Format&)
        {}
    };

    template<class Format>
    struct ScribeTraits<Arca::OpenRelic, Format> final
    {
        using Category = ArcaCompositeScribeCategory<Arca::OpenRelic>;
    };
}