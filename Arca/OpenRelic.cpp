#include "OpenRelic.h"

#include "Reliquary.h"

namespace Arca
{
    OpenRelic::operator Handle() const
    {
        return Handle(id, *owner, Type(Traits<OpenRelic>::typeName, false));
    }

    OpenRelic::operator bool() const
    {
        if (!owner)
            return false;

        return owner->Contains<OpenRelic>(id);
    }

    std::optional<Handle> OpenRelic::Parent() const
    {
        return owner->ParentOf(*this);
    }

    RelicID OpenRelic::ID() const
    {
        return id;
    }

    Reliquary& OpenRelic::Owner() const
    {
        return *owner;
    }

    void OpenRelic::Initialize(Reliquary& owner)
    {
        this->owner = &owner;
    }

    const TypeName Traits<OpenRelic>::typeName = "OpenRelic";
}