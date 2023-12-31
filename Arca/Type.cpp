#include "Type.h"

#include <Inscription/StringScribe.h>
#include <Inscription/NumericScribe.h>

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

namespace Inscription
{
    void Scribe<Arca::Type>::Scriven(ObjectT& object, Format::Binary& format)
    {
        format(object.name);
        format(object.isConst);
    }

    void Scribe<Arca::Type>::Scriven(ObjectT& object, Format::Json& format)
    {
        format("name", object.name);
        format("isConst", object.isConst);
    }
}