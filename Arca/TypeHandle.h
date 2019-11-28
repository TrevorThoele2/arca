#pragma once

#include <string>

#include "Serialization.h"

namespace Arca
{
    using TypeHandleName = std::string;
    struct TypeHandle
    {
        TypeHandleName name{};
        bool isConst = false;

        TypeHandle() = default;
        TypeHandle(TypeHandleName name);
        TypeHandle(TypeHandleName name, bool isConst);
        TypeHandle(const TypeHandle& arg) = default;
        TypeHandle(TypeHandle&& arg) noexcept = default;

        TypeHandle& operator=(const TypeHandle& arg) = default;
        TypeHandle& operator=(TypeHandle&& arg) = default;
    };
}

namespace Chroma
{
    std::string ToString(Arca::TypeHandle typeHandle);
}

namespace Inscription
{
    template<>
    class Scribe<Arca::TypeHandle, BinaryArchive> final
        : public CompositeScribe<Arca::TypeHandle, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };
}