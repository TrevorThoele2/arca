#include "RelicTypeDescription.h"

namespace Arca
{
    RelicTypeDescription::RelicTypeDescription
    (
        TypeHandle typeHandle,
        BaseTypeList baseTypes,
        std::function<SerializationTypeHandles(::Inscription::BinaryArchive&)>&& serializationTypeHandles
    ) :
        typeHandle(std::move(typeHandle)), baseTypes(std::move(baseTypes)), serializationTypeHandles(std::move(serializationTypeHandles))
    {}

    bool RelicTypeDescription::operator==(const RelicTypeDescription& arg) const
    {
        return typeHandle == arg.typeHandle;
    }

    bool RelicTypeDescription::operator!=(const RelicTypeDescription& arg) const
    {
        return !(*this == arg);
    }

    auto RelicTypeDescription::AllSerializationRepresentedTypeHandles(::Inscription::BinaryArchive& archive) const -> SerializationTypeHandles
    {
        return serializationTypeHandles(archive);
    }
}