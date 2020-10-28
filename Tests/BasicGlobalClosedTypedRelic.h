#pragma once

#include <Arca/Relic.h>

class BasicGlobalClosedTypedRelic
{
public:
    int integer = 0;
    std::string string;

    BasicGlobalClosedTypedRelic() = default;

    BasicGlobalClosedTypedRelic(int integer, const std::string& string) : integer(integer), string(string)
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