#pragma once

#include <Arca/ClosedTypedRelic.h>

class BasicLocalClosedTypedRelic : public Arca::ClosedTypedRelic<BasicLocalClosedTypedRelic>
{
public:
    int integer = 0;
    std::string string;

    explicit BasicLocalClosedTypedRelic(Init init) : ClosedTypedRelic(init)
    {}

    BasicLocalClosedTypedRelic(Init init, int integer, const std::string& string) :
        ClosedTypedRelic(init), integer(integer), string(string)
    {}
};

namespace Arca
{
    template<>
    struct Traits<BasicLocalClosedTypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "BasicLocalClosedTypedRelic";
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