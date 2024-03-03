#pragma once

#include <vector>
#include "TypeHandle.h"
#include <Inscription/TypeHandle.h>
#include <Inscription/BinaryArchive.h>

namespace Arca
{
    class RelicTypeDescription
    {
    public:
        using SerializationTypeHandles = std::vector<::Inscription::TypeHandle>;
    public:
        const TypeHandle typeHandle;

        using BaseTypeList = std::vector<RelicTypeDescription>;
        const BaseTypeList baseTypes;

        RelicTypeDescription
        (
            TypeHandle typeHandle,
            BaseTypeList baseTypes,
            std::function<SerializationTypeHandles(::Inscription::BinaryArchive&)>&& serializationTypeHandles
        );

        bool operator==(const RelicTypeDescription& arg) const;
        bool operator!=(const RelicTypeDescription& arg) const;

        SerializationTypeHandles AllSerializationRepresentedTypeHandles(::Inscription::BinaryArchive& archive) const;
    private:
        std::function<SerializationTypeHandles(::Inscription::BinaryArchive&)> serializationTypeHandles;
    };
}