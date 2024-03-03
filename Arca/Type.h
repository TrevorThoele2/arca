#pragma once

#include <string>

namespace Arca
{
    using TypeName = std::string;
    struct Type
    {
        TypeName name{};
        bool isConst = false;

        Type() = default;
        explicit Type(TypeName name);
        Type(TypeName name, bool isConst);
        Type(const Type& arg) = default;
        Type(Type&& arg) noexcept = default;

        Type& operator=(const Type& arg) = default;
        Type& operator=(Type&& arg) = default;

        bool operator==(const Type& arg) const;
        bool operator!=(const Type& arg) const;
    };
}

namespace Chroma
{
    std::string ToString(Arca::Type type);
}