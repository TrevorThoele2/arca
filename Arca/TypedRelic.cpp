#include "TypedRelic.h"

namespace Arca
{
    RelicID TypedRelic::ID() const
    {
        return id;
    }

    Reliquary& TypedRelic::Owner()
    {
        return *owner;
    }

    const Reliquary& TypedRelic::Owner() const
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