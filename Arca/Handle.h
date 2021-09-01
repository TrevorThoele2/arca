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
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {
            archive("id", object.id);
            archive("type", object.type);
        }
    };

    template<class Archive>
    struct ScribeTraits<Arca::Handle, Archive> final
    {
        using Category = CompositeScribeCategory<Arca::Handle>;
    };
}