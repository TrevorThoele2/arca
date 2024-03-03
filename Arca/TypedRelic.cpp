#include "TypedRelic.h"

#include "Reliquary.h"

namespace Arca
{
    TypedRelic::operator RelicHandle() const
    {
        return RelicHandle(id, *owner);
    }

    void TypedRelic::ParentTo(const RelicHandle& parent) const
    {
        owner->ParentRelic(parent, *this);
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