#pragma once

#include "RelicID.h"
#include "Type.h"

#include "Serialization.h"

namespace Arca
{
    class Handle
    {
    public:
        RelicID id = nullRelicID;
        Type type;

        bool operator==(const Handle& arg) const;
        bool operator!=(const Handle& arg) const;
    };
}

namespace Inscription
{
    template<>
    class Scribe<Arca::Handle> final
    {
    public:
        using ObjectT = Arca::Handle;
    public:
        template<class Format>
        void Scriven(ObjectT& object, Format& format)
        {
            format("id", object.id);
            format("type", object.type);
        }
    };

    template<class Format>
    struct ScribeTraits<Arca::Handle, Format> final
    {
        using Category = CompositeScribeCategory<Arca::Handle>;
    };
}