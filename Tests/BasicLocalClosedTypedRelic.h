#pragma once

#include <Arca/Relic.h>

class BasicLocalClosedTypedRelic
{
public:
    int integer = 0;
    std::string string;

    BasicLocalClosedTypedRelic() = default;

    BasicLocalClosedTypedRelic(int integer, const std::string& string) :
        integer(integer), string(string)
    {}
};

namespace Arca
{
    template<>
    struct Traits<BasicLocalClosedTypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static TypeName TypeName() { return "BasicLocalClosedTypedRelic"; }
    };
}

namespace Inscription
{
    template<>
    class Scribe<BasicLocalClosedTypedRelic> final
    {
    public:
        using ObjectT = BasicLocalClosedTypedRelic;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {
            archive("integer", object.integer);
            archive("string", object.string);
        }
    };

    template<class Archive>
    struct ScribeTraits<BasicLocalClosedTypedRelic, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<BasicLocalClosedTypedRelic>;
    };
}