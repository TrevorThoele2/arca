#pragma once

#include <string>

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

        bool operator==(const TypeHandle& arg) const;
        bool operator!=(const TypeHandle& arg) const;
    };
}

namespace Chroma
{
    std::string ToString(Arca::TypeHandle typeHandle);
}