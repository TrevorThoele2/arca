#include "ClosedTypedRelic.h"

#include "Reliquary.h"

namespace Arca
{
    ClosedTypedRelic::operator Handle() const
    {
        return Handle(id, *owner, Type());
    }

    ClosedTypedRelic::operator bool() const
    {
        if (!owner)
            return false;

        return ReliquaryContainsSelf();
    }

    std::optional<Handle> ClosedTypedRelic::Parent() const
    {
        return owner->ParentOf(*this);
    }

    RelicID ClosedTypedRelic::ID() const
    {
        return id;
    }

    Reliquary& ClosedTypedRelic::Owner() const
    {
        return *owner;
    }

    void ClosedTypedRelic::Initialize(Reliquary& owner)
    {
        this->owner = &owner;
        InitializeImplementation();
    }

    ClosedTypedRelic::~ClosedTypedRelic() = default;
}