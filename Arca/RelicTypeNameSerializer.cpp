#include "RelicTypeNameSerializer.h"

#include <cassert>

#include <Inscription/ContainerSize.h>

#include <Chroma/Function.h>

namespace Arca
{
    void RelicTypeNameSerializer::AddAll(const RelicTypeDescriptionGroup& group)
    {
        auto currentId = ID(0);
        for (auto& loop : group)
        {
            if (Has(loop.typeHandle))
                continue;

            entries.push_back(Entry(loop.typeHandle, currentId));
            ++currentId;
        }
    }

    void RelicTypeNameSerializer::Clear()
    {
        entries.clear();
    }

    void RelicTypeNameSerializer::SaveAll(::Inscription::OutputBinaryArchive& archive)
    {
        ::Inscription::ContainerSize size(entries.size());
        archive(size);

        for (auto& loop : entries)
        {
            archive(loop.typeHandle);
            archive(loop.id);
        }
    }

    void RelicTypeNameSerializer::LoadAll(::Inscription::InputBinaryArchive& archive)
    {
        ::Inscription::ContainerSize size;
        archive(size);

        while (size-- > 0)
        {
            TypeHandle typeHandle;
            archive(typeHandle);

            ID id;
            archive(id);

            entries.push_back(Entry(typeHandle, id));
        }
    }

    void RelicTypeNameSerializer::Save(const TypeHandle& typeHandle, ::Inscription::OutputBinaryArchive& archive)
    {
        auto found = std::find_if(
            entries.begin(),
            entries.end(),
            [&typeHandle](const Entry& entry) { return entry.typeHandle == typeHandle; });
        assert(found != entries.end());
        
        archive(found->id);
    }

    void RelicTypeNameSerializer::Load(TypeHandle& typeHandle, ::Inscription::InputBinaryArchive& archive)
    {
        ID id;
        archive(id);

        auto found = std::find_if(
            entries.begin(),
            entries.end(),
            [&id](const Entry& entry) { return entry.id == id; });
        assert(found != entries.end());

        typeHandle = found->typeHandle;
    }

    RelicTypeNameSerializer::Entry::Entry(const TypeHandle& typeHandle, ID id) : typeHandle(typeHandle), id(id)
    {}

    bool RelicTypeNameSerializer::Has(const TypeHandle& typeHandle)
    {
        for (auto& entryLoop : entries)
        {
            if (entryLoop.typeHandle == typeHandle)
                return true;
        }

        return false;
    }
}