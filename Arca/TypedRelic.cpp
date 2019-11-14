#include "TypedRelic.h"

namespace Arca
{
    RelicID TypedRelic::ID() const
    {
        return id;
    }

    TypedRelic::~TypedRelic() = default;

    TypedRelic::TypedRelic(const ::Inscription::BinaryTableData<TypedRelic>& data) :
        id(data.id)
    {}
}