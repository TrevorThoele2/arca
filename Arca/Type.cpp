#include "Type.h"

namespace Arca
{
    Type::Type(TypeName name) : name(std::move(name))
    {}

    Type::Type(TypeName name, bool isConst)
        : name(std::move(name)), isConst(isConst)
    {}

    bool Type::operator==(const Type& arg) const
    {
        return name == arg.name && isConst == arg.isConst;
    }

    bool Type::operator!=(const Type& arg) const
    {
        return !(*this == arg);
    }
}

namespace Chroma
{
    std::string ToString(const Arca::Type& type)
    {
        return !type.isConst
            ? type.name
            : "const " + type.name;
    }
}