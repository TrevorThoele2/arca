#pragma once

#include <vector>
#include "TypeHandle.h"
#include <Inscription/TypeHandle.h>
#include <Inscription/BinaryArchive.h>

namespace Arca
{
    class CuratorTypeDescription
    {
    public:
        using SerializationTypeHandles = std::vector<::Inscription::TypeHandle>;
    public:
        const TypeHandle typeHandle;

        CuratorTypeDescription(TypeHandle typeHandle, std::function<SerializationTypeHandles(::Inscription::BinaryArchive&)>&& serializationTypeHandles);

        bool operator==(const CuratorTypeDescription& arg) const;
        bool operator!=(const CuratorTypeDescription& arg) const;

        SerializationTypeHandles AllSerializationInputTypeHandles(::Inscription::BinaryArchive& archive) const;
    private:
        std::function<SerializationTypeHandles(::Inscription::BinaryArchive&)> serializationTypeHandles;
    };
}