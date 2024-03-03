#pragma once

#include <Arca/ClosedTypedRelic.h>

class BasicGlobalClosedTypedRelic : public Arca::ClosedTypedRelic<BasicGlobalClosedTypedRelic>
{
public:
    int integer = 0;
    std::string string;

    explicit BasicGlobalClosedTypedRelic(Init init) : ClosedTypedRelic(init)
    {}

    BasicGlobalClosedTypedRelic(Init init, int integer, const std::string& string) :
        ClosedTypedRelic(init), integer(integer), string(string)
    {}
};

namespace Arca
{
    template<>
    struct Traits<BasicGlobalClosedTypedRelic>
    {
        static const ObjectType objectType = ObjectType::Relic;
        static inline const TypeName typeName = "BasicGlobalClosedTypedRelic";
        static const Locality locality = Locality::Global;
    };
}

namespace Inscription
{
    template<>
    class Scribe<BasicGlobalClosedTypedRelic> final
    {
    public:
        using ObjectT = BasicGlobalClosedTypedRelic;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {
            archive("integer", object.integer);
            archive("string", object.string);
        }
    };

    template<class Archive>
    struct ScribeTraits<BasicGlobalClosedTypedRelic, Archive> final
    {
        using Category = ArcaCompositeScribeCategory<BasicGlobalClosedTypedRelic>;
    };
}