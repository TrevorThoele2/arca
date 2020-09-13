#pragma once

#include "RelicID.h"
#include "Type.h"
#include "HandleObjectType.h"

#include "Serialization.h"

namespace Arca
{
    class HandleSlim
    {
    public:
        HandleSlim() = default;
        HandleSlim(RelicID id, Type type, HandleObjectType objectType);

        bool operator==(const HandleSlim& arg) const;
        bool operator!=(const HandleSlim& arg) const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Type Type() const;
        [[nodiscard]] HandleObjectType ObjectType();
    private:
        RelicID id = 0;
        Arca::Type type;
        HandleObjectType objectType = HandleObjectType::Relic;
    private:
        INSCRIPTION_ACCESS;
    };
}

namespace Inscription
{
    template<>
    class Scribe<Arca::HandleSlim> final
    {
    public:
        using ObjectT = Arca::HandleSlim;
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
    struct ScribeTraits<Arca::HandleSlim, Archive> final
    {
        using Category = CompositeScribeCategory<Arca::HandleSlim>;
    };
}