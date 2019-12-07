#include "ClosedRelic.h"

#include "Reliquary.h"

namespace Arca
{
    ClosedRelic::operator bool() const
    {
        if (!owner)
            return false;

        return owner->Contains<ClosedRelic>(id);
    }

    std::optional<Handle> ClosedRelic::Parent() const
    {
        return owner->ParentOf(AsHandle(*this));
    }

    RelicID ClosedRelic::ID() const
    {
        return id;
    }

    Reliquary& ClosedRelic::Owner() const
    {
        return *owner;
    }

    void ClosedRelic::Initialize(Reliquary& owner)
    {
        this->owner = &owner;
    }

    const TypeName Traits<ClosedRelic>::typeName = "ClosedRelic";
}