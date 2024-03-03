#pragma once

#include "Openness.h"

#include "CompositeScribeCategory.h"

namespace Arca
{
    template<Openness opennessValue>
    class Relic final
    {
    public:
        Relic() = default;
    };

    template<Openness opennessValue>
    struct Traits<Relic<opennessValue>>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName()
        {
            return "Arca::Relic<" +
                Arca::TypeName(opennessValue == Openness::Closed
                    ? "Closed"
                    : "Open") +
                ">";
        }
        static const Openness openness = opennessValue;
    };

    using OpenRelic = Relic<Openness::Open>;
    using ClosedRelic = Relic<Openness::Closed>;
}

namespace Inscription
{
    template<Arca::Openness opennessValue>
    class Scribe<Arca::Relic<opennessValue>> final
    {
    public:
        using ObjectT = Arca::Relic<opennessValue>;
    public:
        template<class Archive>
        void Scriven(ObjectT&, Archive&)
        {}
    };

    template<Arca::Openness opennessValue, class Archive>
    struct ScribeTraits<Arca::Relic<opennessValue>, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<Arca::Relic<opennessValue>>;
    };
}