#pragma once

#include "RelicID.h"
#include "Type.h"
#include "SlimHandle.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    class Handle
    {
    public:
        Handle() = default;
        Handle(RelicID id, Reliquary* owner, Type type, HandleObjectType objectType);
        Handle(RelicID id, Reliquary& owner, Type type, HandleObjectType objectType);

        [[nodiscard]] bool operator==(const Handle& arg) const;
        [[nodiscard]] bool operator!=(const Handle& arg) const;

        [[nodiscard]] operator SlimHandle() const;

        [[nodiscard]] Reliquary* Owner() const;
        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Type Type() const;
        [[nodiscard]] HandleObjectType ObjectType() const;
    private:
        RelicID id = nullRelicID;
        Reliquary* owner = nullptr;
        Arca::Type type;
        HandleObjectType objectType = HandleObjectType::Relic;
    private:
        INSCRIPTION_ACCESS;
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

            if (archive.IsInput())
            {
                const auto context = archive.template UserContext<ReliquaryUserContext>();
                object.owner = context->reliquary;

                const auto objectType = context->reliquary->ObjectHandleTypeFor(object.type.name);
                object.objectType = *objectType;
            }
        }
    };

    template<class Archive>
    struct ScribeTraits<Arca::Handle, Archive> final
    {
        using Category = CompositeScribeCategory<Arca::Handle>;
    };
}