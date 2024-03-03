#pragma once

#include "RelicID.h"
#include "Type.h"
#include "HandleObjectType.h"

#include "Serialization.h"

namespace Arca
{
    class SlimHandle
    {
    public:
        SlimHandle() = default;
        SlimHandle(RelicID id, Type type, HandleObjectType objectType);

        bool operator==(const SlimHandle& arg) const;
        bool operator!=(const SlimHandle& arg) const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Type Type() const;
        [[nodiscard]] HandleObjectType ObjectType();
    private:
        RelicID id = nullRelicID;
        Arca::Type type;
        HandleObjectType objectType = HandleObjectType::Relic;
    private:
        INSCRIPTION_ACCESS;
    };
}

namespace Inscription
{
    template<>
    class Scribe<Arca::SlimHandle> final
    {
    public:
        using ObjectT = Arca::SlimHandle;
    public:
        template<class Archive>
        void Scriven(ObjectT& object, Archive& archive)
        {
            archive("id", object.id);
            archive("type", object.type);

            if (archive.IsInput())
            {
                const auto reliquary = archive.template UserContext<ReliquaryUserContext>()->reliquary;

                const auto objectType = reliquary->ObjectHandleTypeFor(object.type.name);
                object.objectType = *objectType;
            }
        }
    };

    template<class Archive>
    struct ScribeTraits<Arca::SlimHandle, Archive> final
    {
        using Category = CompositeScribeCategory<Arca::SlimHandle>;
    };
}