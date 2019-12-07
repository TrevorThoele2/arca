#include "OpenTypedRelic.h"

#include "Reliquary.h"

namespace Arca
{
    OpenTypedRelic::operator Handle() const
    {
        return Handle(id, *owner, TypeHandle());
    }

    OpenTypedRelic::operator bool() const
    {
        if (!owner)
            return false;

        return ReliquaryContainsSelf();
    }

    std::optional<Handle> OpenTypedRelic::Parent() const
    {
        return owner->ParentOf(*this);
    }

    RelicID OpenTypedRelic::ID() const
    {
        return id;
    }

    Reliquary& OpenTypedRelic::Owner() const
    {
        return *owner;
    }

    void OpenTypedRelic::Initialize(Reliquary& owner)
    {
        this->owner = &owner;
        InitializeImplementation();
    }

    OpenTypedRelic::~OpenTypedRelic() = default;
}