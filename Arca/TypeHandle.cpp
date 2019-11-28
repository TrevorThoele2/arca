#include "TypeHandle.h"

namespace Arca
{
    TypeHandle::TypeHandle(TypeHandleName name) : name(std::move(name)), isConst(false)
    {}

    TypeHandle::TypeHandle(TypeHandleName name, bool isConst)
        : name(std::move(name)), isConst(isConst)
    {}
}

namespace Chroma
{
    std::string ToString(Arca::TypeHandle typeHandle)
    {
        return !typeHandle.isConst
            ? typeHandle.name
            : "const " + typeHandle.name;
    }
}

namespace Inscription
{
    void Scribe<Arca::TypeHandle, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        archive(object.name);
        archive(object.isConst);
    }
}