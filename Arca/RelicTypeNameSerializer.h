#pragma once

#include <vector>

#include "TypeHandle.h"
#include "RelicTypeDescriptionGroup.h"

#include "Serialization.h"

namespace Arca
{
    class RelicTypeNameSerializer
    {
    public:
        void AddAll(const RelicTypeDescriptionGroup& group);

        void Clear();
    public:
        void SaveAll(::Inscription::OutputBinaryArchive& archive);
        void LoadAll(::Inscription::InputBinaryArchive& archive);
        void Save(const TypeHandle& typeHandle, ::Inscription::OutputBinaryArchive& archive);
        void Load(TypeHandle& typeHandle, ::Inscription::InputBinaryArchive& archive);
    private:
        using ID = size_t;

        struct Entry
        {
            TypeHandle typeHandle;
            ID id;
            Entry(const TypeHandle& typeHandle, ID id);
        };
    private:
        std::vector<Entry> entries;

        bool Has(const TypeHandle& typeHandle);
    };
}