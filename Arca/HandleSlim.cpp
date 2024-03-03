#include "HandleSlim.h"

#include "Reliquary.h"

namespace Arca
{
    HandleSlim::HandleSlim(RelicID id, Arca::Type type, HandleObjectType objectType)
        : id(id), type(std::move(type)), objectType(objectType)
    {}

    bool HandleSlim::operator==(const HandleSlim& arg) const
    {
        return id == arg.id && type == arg.type;
    }

    bool HandleSlim::operator!=(const HandleSlim& arg) const
    {
        return !(*this == arg);
    }

    RelicID HandleSlim::ID() const
    {
        return id;
    }

    Type HandleSlim::Type() const
    {
        return type;
    }

    HandleObjectType HandleSlim::ObjectType()
    {
        return objectType;
    }
}

namespace Inscription
{
    void Scribe<Arca::HandleSlim, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        archive(object.id);
        archive(object.type);

        if (archive.IsInput())
        {
            const auto reliquary = archive.UserContext<Arca::InscriptionUserContext>()->reliquary;

            const auto objectType = reliquary->ObjectHandleTypeFor(object.type.name);
            object.objectType = *objectType;
        }
    }
}