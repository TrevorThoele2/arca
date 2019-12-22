#include "OpenTypedRelic.h"

#include "Reliquary.h"

namespace Arca
{
    OpenTypedRelic::operator bool() const
    {
        if (!owner)
            return false;

        return ReliquaryContainsSelf();
    }

    std::optional<Handle> OpenTypedRelic::Parent() const
    {
        return owner->ParentOf(Handle(ID(), Owner(), Type(), HandleObjectType::Relic));
    }

    RelicID OpenTypedRelic::ID() const
    {
        return id;
    }

    Reliquary& OpenTypedRelic::Owner() const
    {
        return *owner;
    }

    OpenTypedRelic::~OpenTypedRelic() = default;
}