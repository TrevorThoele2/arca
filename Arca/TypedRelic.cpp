#include "TypedRelic.h"

#include "Reliquary.h"

namespace Arca
{
    TypedRelic::operator Handle() const
    {
        return Handle(id, *owner);
    }

    TypedRelic::operator bool() const
    {
        if (!owner)
            return false;

        return ReliquaryContainsSelf();
    }

    std::optional<Handle> TypedRelic::Parent() const
    {
        return owner->ParentOf(*this);
    }

    RelicID TypedRelic::ID() const
    {
        return id;
    }

    Reliquary& TypedRelic::Owner() const
    {
        return *owner;
    }

    void TypedRelic::Initialize(Reliquary& owner)
    {
        this->owner = &owner;
        InitializeImplementation();
    }

    TypedRelic::~TypedRelic() = default;
}