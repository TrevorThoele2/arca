#pragma once

#include "TypeHandle.h"
#include "Serialization.h"

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