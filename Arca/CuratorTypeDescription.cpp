#include "CuratorTypeDescription.h"

namespace Arca
{
    CuratorTypeDescription::CuratorTypeDescription(
        TypeHandle typeHandle, std::function<SerializationTypeHandles(::Inscription::BinaryArchive&)>&& serializationTypeHandles)
        :
        typeHandle(std::move(typeHandle)), serializationTypeHandles(std::move(serializationTypeHandles))
    {}

    bool CuratorTypeDescription::operator==(const CuratorTypeDescription& arg) const
    {
        return typeHandle == arg.typeHandle;
    }

    bool CuratorTypeDescription::operator!=(const CuratorTypeDescription& arg) const
    {
        return !(*this == arg);
    }

    auto CuratorTypeDescription::AllSerializationRepresentedTypeHandles(::Inscription::BinaryArchive& archive) const -> SerializationTypeHandles
    {
        return serializationTypeHandles(archive);
    }
}