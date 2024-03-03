#include "ClosedRelic.h"

#include "Reliquary.h"

namespace Arca
{
    ClosedRelic::operator Handle() const
    {
        return Handle(ID(), Owner(), TypeHandle(Traits<ClosedRelic>::typeName, false));
    }

    ClosedRelic::operator bool() const
    {
        if (!owner)
            return false;

        return owner->Contains<ClosedRelic>(id);
    }

    std::optional<Handle> ClosedRelic::Parent() const
    {
        return owner->ParentOf(*this);
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

    const TypeHandleName Traits<ClosedRelic>::typeName = "ClosedRelic";
}

namespace Inscription
{
    void Scribe<Arca::ClosedRelic, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {}
}