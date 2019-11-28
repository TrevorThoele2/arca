#include "TypedRelic.h"

#include "Reliquary.h"

namespace Arca
{
    TypedRelic::operator Handle() const
    {
        return Handle(id, *owner);
    }

    void TypedRelic::ParentTo(const Handle& parent) const
    {
        owner->ParentRelicTo(parent, *this);
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

    TypedRelic::TypedRelic(const ::Inscription::BinaryTableData<TypedRelic>& data) :
        id(data.id)
    {}
}