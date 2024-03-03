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
        static constexpr ObjectType objectType = ObjectType::Relic;
        static const inline TypeName typeName = "BasicLocalClosedTypedRelic";
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
        template<class Format>
        void Scriven(ObjectT& object, Format& format)
        {
            format("integer", object.integer);
            format("string", object.string);
        }
    };

    template<class Format>
    struct ScribeTraits<BasicLocalClosedTypedRelic, Format> final
    {
        using Category = ArcaCompositeScribeCategory<BasicLocalClosedTypedRelic>;
    };
}